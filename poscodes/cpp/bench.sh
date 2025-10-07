#!/usr/bin/env bash
# bench.sh - corre poscodes y genera results.csv y best.csv
# Uso: RUNS=3 ./bench.sh
set -u

cd "$(dirname "$0")/build" || { echo "No existe cpp/build"; exit 1; }

BIN=./poscodes
OUT=results.csv
BEST=best.csv
RUNS=${RUNS:-3}

echo "algo,n,file,rep,time_s,nps,sorted,rss_kb" > "$OUT"

datasets=(
  "../../codes_500K.txt:500000"
  "../../codes_1M.txt:1000000"
  "../../codes_10M.txt:10000000"
)

algos=(radix merge quick)

for ds in "${datasets[@]}"; do
  IFS=: read -r file n <<< "$ds"
  if [[ ! -f "$file" ]]; then
    echo "No existe $file, lo salto."
    continue
  fi
  for algo in "${algos[@]}"; do
    for rep in $(seq 1 "$RUNS"); do
      out=$(/usr/bin/time -v "$BIN" --algo="$algo" --file="$file" --n="$n" --print=0 2>&1)

      # 1) tiempo (primero el que imprime el binario)
      time=$(printf "%s\n" "$out" | sed -n 's/.* time=\([0-9.]\+\) s.*/\1/p' | head -n1)

      # 2) si no, toma el "Elapsed" de /usr/bin/time (en inglés o español)
      if [[ -z "$time" ]]; then
        wall=$(printf "%s\n" "$out" | awk -F': ' '
          /Elapsed \(wall clock\) time/ {print $2; exit}
          /Tiempo total transcurrido/   {print $2; exit}
        ')
        if [[ -n "$wall" ]]; then
          wall=${wall//,/.}
          IFS=':' read -r a b c <<< "$wall"
          if [[ -z "$c" ]]; then
            time=$(awk -v mm="$a" -v ss="$b" 'BEGIN{print mm*60 + ss}')
          else
            time=$(awk -v hh="$a" -v mm="$b" -v ss="$c" 'BEGIN{print hh*3600 + mm*60 + ss}')
          fi
        fi
      fi

      sorted=$(printf "%s\n" "$out" | awk '/^Sorted\?/{print $2; exit}')
      rss=$(printf "%s\n" "$out" | awk -F: '
        /Maximum resident set size/                 {gsub(/ /,"",$2); print $2; exit}
        /Tamaño máximo de conjunto residente/      {gsub(/ /,"",$2); print $2; exit}
      ')
      nps=$(awk -v n="$n" -v t="${time:-0}" 'BEGIN{if(t>0) printf "%.2f", n/t; else print "NaN"}')

      echo "$algo,$n,$file,$rep,${time:-},$nps,${sorted:-},${rss:-}" >> "$OUT"
    done
  done
done

awk -F, 'NR==1{print; next} {k=$1","$2","$3; if(!(k in min) || $5<min[k]){min[k]=$5; row[k]=$0}} END{for(k in row) print row[k]}' "$OUT" \
| sort -t, -k1,1 -k2,2n > "$BEST"

echo "CSV completo : $(pwd)/$OUT"
echo "Mejores por caso: $(pwd)/$BEST"
