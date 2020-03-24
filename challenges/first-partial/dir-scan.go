package main

import (
	"fmt"
	"os"
	"text/tabwriter"
	"path/filepath"
)

func printTable(dir string ,d, s, dev, l, oth int ) {
	tw := tabwriter.NewWriter(os.Stdout, 8, 8, 0, '\t', 0)
	fmt.Fprintf(tw, "+\t+\t+\n")
	fmt.Fprintf(tw, "|Path:\t| %s\t|\n", dir)
	fmt.Fprintf(tw, "+\t+\t+\n")
	fmt.Fprintf(tw, "| Directories:\t| %d\t|\n", d)
	fmt.Fprintf(tw, "| Symbolic Links:\t| %d\t|\n", l)
	fmt.Fprintf(tw, "| Devices:\t| %d\t|\n", dev)
	fmt.Fprintf(tw, "| Sockets:\t| %d\t|\n", s)
	fmt.Fprintf(tw, "| Other files:\t| %d\t|\n", oth)
	fmt.Fprintf(tw, "+\t+\t+\n")
	tw.Flush()
}

// scanDir stands for the directory scanning implementation
func scanDir(dir string) error {
	var directories, sockets, devices, links, others int = 0,0,0,0,0

	countFile := func(absPath string,metaData os.FileInfo,err error) error{
		if err != nil { return err }
		
		switch typeOfFile := metaData.Mode(); {
			case typeOfFile.IsDir():
				directories++
			case typeOfFile & os.ModeSymlink != 0:
				links++
			case typeOfFile & os.ModeSocket != 0:
				sockets++
			case typeOfFile & os.ModeDevice != 0:
				devices++
			default:
				others++
		}
		return nil
	}

	err := filepath.Walk(dir, countFile)
	if(err != nil){ return err }
	printTable(dir, directories, sockets, devices, links, others)
	return nil
}

func main() {

	if len(os.Args) < 2 {
		fmt.Println("Usage: ./dir-scan <directory>")
		os.Exit(1)
	}

	err:= scanDir(os.Args[1])
	if(err != nil){ fmt.Println("There were an error while scanning. Check your path is correct.")}
}
