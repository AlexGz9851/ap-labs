package main

import (
	"fmt"
	"net"
	"strings"
	"os"
    /*"os/exec"
    "runtime"*/
)

/*
init and callClear functions were obtined from 
https://stackoverflow.com/questions/22891644/how-can-i-clear-the-terminal-screen-in-go?fbclid=IwAR21eF8uuO2r9q2O9poitDjj3QarSl_SBmFdD3Q6SK6ZSs6Fo4EwDH5t254
and those functions are just for clean command line screen purposes.

var clear map[string]func() //create a map for storing clear funcs

func init() {
    clear = make(map[string]func()) //Initialize it
    clear["linux"] = func() { 
        cmd := exec.Command("clear") //Linux example, its tested
        cmd.Stdout = os.Stdout
        cmd.Run()
    }
    clear["windows"] = func() {
        cmd := exec.Command("cmd", "/c", "cls") //Windows example, its tested 
        cmd.Stdout = os.Stdout
        cmd.Run()
    }
}

func CallClear() {
    value, ok := clear[runtime.GOOS] //runtime.GOOS -> linux, windows, darwin etc.
    if ok { //if we defined a clear func for that platform:
        value()  //we execute it
    } else { //unsupported platform
        panic("Your platform is unsupported! I can't clear terminal screen :(")
    }
}
*/
type localClock struct{
	location string
	connection net.Conn
}

func (c localClock) handleConn(){
	
	//for  {
		time := make([]byte, len("15:04:05\n"))
		_, err := c.connection.Read(time)
		if(err!=nil){
			fmt.Printf("%s\n",err)
		}else{
			/*CallClear()*/
			fmt.Printf("Local time in %s: %s\n" ,c.location, time)
		}
	//}
	c.connection.Close()
}


func main(){
	if(len(os.Args)< 2){
		/*There is not any location-port setted.
		Due we don't know if in  the server side there is a default 
		server, we cannot set a default port.*/
		fmt.Printf("Please, set the port server you want to connect. Aborted.\n")
		return
	}

	for _, server:= range os.Args[1:]{
		var data = strings.Split(server, "=")
		if(len(data)!=2){
			fmt.Printf("Insert the input properly. Aborted.\n")
			return
		}
		conn, err := net.Dial("tcp", data[1])
		if(err!=nil){
			fmt.Printf("Error in connection. Aborted.\n")
			return
		}
		myClock := localClock{
			location :data[0],
			connection : conn,
		}
		myClock.handleConn()

	}

}