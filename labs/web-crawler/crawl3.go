// Copyright © 2016 Alan A. A. Donovan & Brian W. Kernighan.
// License: https://creativecommons.org/licenses/by-nc-sa/4.0/

// See page 241.

// Crawl2 crawls web links starting with the command-line arguments.
//
// This version uses a buffered channel as a counting semaphore
// to limit the number of concurrent calls to links.Extract.
//
// Crawl3 adds support for depth limiting.
//
package main

import (
	"fmt"
	"flag"
	"log"
	"os"

	"gopl.io/ch5/links"
)

var tokens = make(chan struct{}, 20)

func crawl(url string) []string {
	fmt.Println(url)
	tokens <- struct{}{} // acquire a token
	list, err := links.Extract(url)
	<-tokens // release the token

	if err != nil {
		log.Print(err)
	}
	return list
}

var seen = make(map[string]bool)

func crawlDepth(depth int, url string, done chan bool){
	if(depth==0){
		done <- true
		return
	}
	visited, ok := seen[url]
	if visited && ok {
		done <- true
		return
	}else{
		seen[url] = true
	}

	links := crawl(url)
	linksDone := make(chan bool)
	for _, link := range links {
		go crawlDepth(depth-1, link, linksDone)
		<-linksDone
	}
	done <- true
}

func main() {
	
	depth := flag.Int("depth", 1, "Sets the depth of the url crawling.")
	flag.Parse()
	if len(os.Args) != 3{
		fmt.Println("Wrong number of arguments. Aboerted.")
		return
	}
	url := os.Args[2]
	done := make(chan bool)
	seen[url] = false
	go crawlDepth(*depth, url, done)
	<-done
}
