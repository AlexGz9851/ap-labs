package main

import (
	"strings"
	"golang.org/x/tour/wc"
)

func WordCount(s string) map[string]int {
    words := make(map[string]int)
	for _, val := range strings.Fields(s){
		words[val]++
	}
	return words
}

func main() {
	wc.Test(WordCount)
}
