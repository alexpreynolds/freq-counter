# freq-counter
We slide windows over a FASTA file and count how many of one or more characters appear in the window. 

Standard output is written in BED format.

## Example

```
$ ./freq-counter --chars="AG" --span=20 --step=1 ../data/GRCh38_no_alts.fa | starch --omit-signature - > ../results/GRCh38_no_alts.counts_AG.starch
```

Replace `AG` with other character sets (`CT` etc.).

The resulting file is very large, so we compress it here with [BEDOPS](https://github.com/bedops/bedops) `starch`.
