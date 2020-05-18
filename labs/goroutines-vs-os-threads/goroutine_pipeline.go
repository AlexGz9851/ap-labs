package main

import (
	"fmt"
	"os"
	"runtime"
	"time"
)

var quit = make(chan struct{})

func main() {
	var pipes  int =  10000000

	file, err := os.Create("pipelines_9_4.txt")
	if err != nil {
		panic(err)
	}

	defer file.Close()

	
	
	in := make(chan struct{})
	out := make(chan struct{})
	start := time.Now().Unix()
	memStatsStart := getMemStats()

	go myRoutine(in, out, 0, pipes)
	in <- struct{}{}
	<-quit

	end := time.Now().Unix()
	memStatsEnd := getMemStats()
	deltaMemStats := substract(memStatsEnd, memStatsStart)
	strStats := fmt.Sprintf("\n\tMem Alloc : %d MiB\n\tCurrent Mallocs : %d\n\tHeapInUse : %dMiB\n\tMemoria en sistema: %dMiB",deltaMemStats[0],deltaMemStats[1],deltaMemStats[2], deltaMemStats[3])

	_, e1 := file.WriteString("Pipeline analysis:\n")
	if e1 != nil {
		panic(err)
	}
	_, e2 := file.WriteString(fmt.Sprintf("It took %d seconds to travel %d pipelines to the dummy object.\n", end-start, pipes))
	if e2 != nil {
		panic(err)
	}
	_, e3 := file.WriteString("The total of memory consumed  by this process was: "  +strStats )
	if e3 != nil {
		panic(err)
	}

	_, e4 := file.WriteString("\n\nNote: the process ran out of stack when it was tested with 20000000 goroutines.\n")
	if e4 != nil {
		panic(err)
	}
}

func getMemStats() [] uint64 {
	var m runtime.MemStats
	runtime.ReadMemStats(&m)
	var  memStats= make([]uint64, 4) 
	memStats[0] = m.Alloc/1048576
	memStats[1] = m.Mallocs-m.Frees
	memStats[2] = m.HeapInuse/1048576
	memStats[3] = (m.Sys)/1048576
	return memStats
}

func substract(a, b [] uint64)  [] uint64{
	if len(a) != len(b) {
		return nil
	}
	var  c= make([]uint64, len(a)) 
	for i:=0;i<len(a);i++{
		c[i] = a[i] -b[i] 
	}
	return c
}


func myRoutine(input chan struct{}, output chan struct{}, current int, total int) {
	if current != total {
		go myRoutine(output, input, current+1, total)
		output <- <-input
	} else {
		quit <- struct{}{}
	}
}
