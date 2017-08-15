
rm result.csv
echo "rollnum,mark" >> result.csv

for file in *.sh; do
    if [ "$file" != "ceed_partsAB_checker.sh" ]; then
          rol=`echo ${file%.*}`
          mkdir $rol
          mv $file $rol
          cp partA.csv $rol
          cp partB.csv $rol
          cd $rol
          sh "$file"
          cd ..
          op=`cmp --silent ./final-output.csv "$rol/final-output.csv" && echo "10" || echo "0"`
          mark=$op
          echo "$rol,$mark" >> result.csv
    fi
done
