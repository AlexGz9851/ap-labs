package main
import (
	"fmt"
	"time"
	"os"
)

var hitsToTheBall int

func pingPong(input, output chan int){
	for x:= range input{
		hitsToTheBall=x
		output <- (x+1)
	}
}

func start(output chan int){ 
	output <- 0
}

func write(file *os.File, output string){
	_, e1 := file.WriteString(fmt.Sprintf("%s\n", output))
	if e1 != nil {
		panic(e1)
	}

}


func main(){
	hitsToTheBall = 0
	timeLapse := 8
	left:= make(chan int)
	right := make(chan int)

	file, err := os.Create("ping_pong_9_5.txt")
	if err != nil {
		panic(err)
	}

	go start(left)
	go pingPong(left, right )
	go pingPong(right, left )

	time.Sleep(time.Duration(timeLapse) * time.Second)	

	write(file,"Ping pong in goroutines")
	write(file,fmt.Sprintf("Total times the ball cross the net: %d", hitsToTheBall))
	write(file,fmt.Sprintf("Time elapsed: %d (secs)", timeLapse))
	write(file,fmt.Sprintf("Average of number of hits the ball got in a second: %d",(hitsToTheBall/timeLapse)))
}