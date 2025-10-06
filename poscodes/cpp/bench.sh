#!/usr/bin/env bash
set -euo pipefail

# Ir a la carpeta de build (donde está el binario)
cd "$(dirname "$0")/build"

BIN=./poscodes
OUT=results.csv
BEST=best.csv
RUNS=${RUNS:-3}   # repeticiones por caso (puedes exportar RUNS=5, etc.)

# Lista de datasets (ruta relativa al build : N líneas)
DATASETS=(
  "../../codes_500K.txt:500000"
  "../../codes_1M.txt:1000000"
  "../../codes_10M.txt:10000000"
)

ALGOS=(radix merge quick)

echo "algo,n,file,rep,time_s,nps,sorted,rss_kb" > "$OUT"

have_time=1
command -v /usr/bin/time >/dev/null 2>&1 || have_time=0
if [[ "$have_time" -eq 0 ]]; then
  echo "⚠️  /usr/bin/time no encontrado. Instala con: sudo apt-get install -y time"
fi

run_case () {
  local algo="$1" file="$2" n="$3" rep="$4"

  if [[ "$have_time" -eq 1 ]]; then
    # Ejecuta y captura stdout+stderr para extraer RSS
    local out
    out=$(/usr/bin/time -v "$BIN" --algo="$algo" --file="$file" --n="$n" --print=0 2>&1)
    local time=$(awk -F'time=' '/time=/{split($2,a," "); print a[1]}' <<< "$out")
    local sorted=$(awk '/^Sorted\?/{print $2}' <<< "$out")
    local rss=$(awk -F: '/Maximum resident set size/{gsub(/ /,"",$2); print $2}' <<< "$out")
  else
    local out
    out=$("$BIN" --algo="$algo" --file="$file" --n="$n" --print=0)
    local time=$(awk -F'time=' '/time=/{split($2,a," "); print a[1]}' <<< "$out")
    local sorted=$(awk '/^Sorted\?/{print $2}' <<< "$out")
    local rss=""
  fi

  # Registros/segundo (nps)
  local nps=$(awk -v n="$n" -v t="$time" 'BEGIN{if(t>0) printf "%.2f", n/t; else print "NaN"}')

  echo "$algo,$n,$file,$rep,$time,$nps,$sorted,$rss" | tee -a "$OUT" >/dev/null
}

for ds in "${DATASETS[@]}"; do
  IFS=: read -r file n <<< "$ds"
  [[ -f "$file" ]] || { echo "⚠️  No existe $file, lo salto."; continue; }

  for algo in "${ALGOS[@]}"; do
    for rep in $(seq 1 "$RUNS"); do
      run_case "$algo" "$file" "$n" "$rep"
    done
  done
done

# Mejores tiempos (best-of-RUNS) por (algo,n,file)
awk -F, '
  NR==1{print $0; next}
  {k=$1","$2","$3; if(!(k in best) || $5 < best[k]) {best[k]=$5; line[k]=$0}}
  END{for(k in line) print line[k]}
' "$OUT" | sort -t, -k1,1 -k2,2n > "$BEST"

echo "✅ Listo:"
echo " - CSV completo : $(pwd)/$OUT"
echo " - Mejores por caso: $(pwd)/$BEST"
