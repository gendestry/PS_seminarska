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
Sekvenčni | 31.3 | 14
OpenMP [1]| 36.9 | 13.6
OpenMP [2]| 24.6 | 10.3
OpenMP [4]| 14.9 | 6.5
OpenMP [8]| 9.6 | 4.7
OpenMP [16]| 7.3 | 4.4
OpenMP [32]| 5.8 | 3.3
OpenMP [64]| 6 | 2.9
OpenCL | / | /

### Predstavitev z matriko
Algoritem | Čas (brez optimizacij) | Čas (O3)
---- |:-----------:|:-----------:
Sekvenčni | 20.3 | 3.1
OpenMP [1]| 21.4 | 10.4
OpenMP [2]| 13.1 | 7.5
OpenMP [4]| 7.8 | 4.9
OpenMP [8]| 4.8 | 3.1
OpenMP [16]| 3.2 | 2.3
OpenMP [32]| 3.2 | 2.6
OpenMP [64]| 3 | 2.3
OpenCL [1]|2.96|2.86
OpenCL [2]|1.76|1.71
OpenCL [4]|1|1
OpenCL [8]|0.63|0.62
OpenCL [16]|0.4|0.4
OpenCL [32]|0.29|0.29
OpenCL [64]|0.23|0.23
OpenCL [128]|0.2|0.2
OpenCL [256]|0.21|0.2