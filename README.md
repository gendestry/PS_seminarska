# PageRank Algorithm - PS seminarska
## Opis naloge

Cilj naloge je implementacija *PageRank* algoritma s pomočjo paralelizacije in primerjava različnih metod paralelizacije.

Algoritem bomo testirali z različnimi knjižnicami: 
 - `OpenMP` - CPU multithreading
 - `OpenCL` - GPU multithreading

Za implementacijo je uporabljen C++17.

## Vhodni podatki
Podatkovna baza povezav spletnih strani (`graph-google.txt`) vsebuje *875713* vozlišč in *5105039* povezav.
Algoritem PageRank do natančnosti `1e-5` konvergira v *47* korakih.

## Predstavitve podatkov
Implementirani sta dve različni predstavitvi; z grafom ter z matriko sosednosti. Matrika sosednosti je torej oblike `875.713x875.713 => 766.873.258.369 elementov`, vendar je zaradi učinkovitejšega zapisa predstavljena kot
redka matrika (hrani le pozicije in vrednosti neničelnih elementov - `5.105.039*3 => 15.315.117`) - na ta način hranimo `~50.000 krat` manj zapisov.

## Implementacija
Ker algoritem PageRank precej hitro konvergira, je pogoj zanke vezan na razliko v ocenah rankov spletnih strani
in ne na arbitrarno število iteracij. Računanje razlike je posledično tudi paralelizirano.

### OpenMP
Paralelizacija je implementirana v razredu matrik - vse operacije nad matrikami se izvajajo paralelno.

### OpenCL
Računanje poteka v 3 korakih:
 - množenje redke matrike sosednosti s prejšnjimi ocenami
 - premik novih ocen in računanje razlike
 - redukcija razlik

## Rezultati testov
Časi so merjeni enkrat in so bolj okvirni kot pa resne meritve.

### Predstavitev z grafom
Algoritem | Čas (brez optimizacij) | Čas (O3)
---- |:-----------:|:-----------:
Sekvenčni | 30,8 ± 0,02 | 13,79 ± 0,006
OpenMP [1]| 37,08 ± 0,17 | 13.52 ± 0,072
OpenMP [2]| 23,56 ± 0,23 | 9,66 ± 0,075
OpenMP [4]| 14,7 ± 0,02 | 6.29 ± 0,036
OpenMP [8]| 9,75 ± 0,14 | 4.89 ± 0,176
OpenMP [16]| 7,61 ± 0,03 | 5.1 ± 0,025
OpenMP [32]| 5,62 ± 0,36 | 3.27 ± 0,011
OpenMP [64]| 4,55 ± 0,04 | 2.73 ± 0,056
OpenCL | / | /

### Predstavitev z matriko
Algoritem | Čas (brez optimizacij) | Čas (O3)
---- |:-----------:|:-----------:
Sekvenčni | 30,84 ± 0,02 | 3.1 ± 0,003
OpenMP [1]| 20,9 ± 0,13 | 8,7 ± 0,32
OpenMP [2]| 12,4 ± 0,09 | 6,1 ± 0,27
OpenMP [4]| 7,4 ± 0,03 | 3,9 ± 0,18
OpenMP [8]| 4,9 ± 0,02 | 2,9 ± 0,2
OpenMP [16]| 3,6 ± 0,04 | 2,76 ± 0,04
OpenMP [32]| 2,49 ± 0,03 | 2.15 ± 0,004
OpenMP [64]| 2,34 ± 0,03 | 2.16 ± 0,018
OpenCL [1]|3,01 ± 0,11|2.96 ± 0,001
OpenCL [2]|1,76 ± 0,002|1.75 ± 0,001
OpenCL [4]|1,04 ± 0,001|1,04 ± 0,0005
OpenCL [8]|0,63 ± 0,001|0.63 ± 0,0004
OpenCL [16]|0,4 ± 0,0004|0.4 ± 0,0003
OpenCL [32]|0,29 ± 0,0002|0.29 ± 0,0002
OpenCL [64]|0,23 ± 0,0005|0.23 ± 0,0003
OpenCL [128]|0,2 ± 0,0003|0.2 ± 0,0001
OpenCL [256]|0,21 ± 0,0001|0.21 ± 0,0002