#!/usr/bin/perl

open(DATA, "<au_22nm_sputtered.mat.txt") or die "Couldn't open file file.txt, $!";

$count = 0;
while($line = <DATA>) {

   if($line =~ /((\d+)\s+(\d+.\d+)\s+(\d+.\d+))/)
   {
      print "imaginary_parts[$count] = {$3 + $4i};   \n";
      $count++;
   }
}

print "\n";
