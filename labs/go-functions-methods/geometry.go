// Copyright © 2016 Alan A. A. Donovan & Brian W. Kernighan.
// License: https://creativecommons.org/licenses/by-nc-sa/4.0/

// See page 156.

// Package geometry defines simple types for plane geometry.
//!+point
package main

import (
	"math"
	"math/rand"
	"os"
	"strconv"
	"fmt"
	"time"
)
type Point struct{ x, y float64 }

// traditional function
func Distance(p, q Point) float64 {
	return math.Hypot(q.x-p.x, q.y-p.y)
}

// same thing, but as a method of the Point type
func (p Point) Distance(q Point) float64 {
	return math.Hypot(q.x-p.x, q.y-p.y)
}

//!-point
func (p Point) X() float64{
	return p.x
}

func (p Point) Y() float64{
	return p.y
}
//!+path

// A Path is a journey connecting the points with straight lines.
type Path []Point

// Distance returns the distance traveled along the path.
func (path Path) Distance() float64 {
	sum := 0.0
	for i := range path {
		if i > 0 {
			sum += path[i-1].Distance(path[i])
		}
	}
	return sum
}

//!-path

//---------------------


// Given three colinear points p, q, r, the function checks if 
// point q lies on line segment 'pr' 
func onSegment( p, q, r Point)  bool{ 
	return (q.x <= math.Max(p.x, r.x) && q.x >= math.Min(p.x, r.x) && 
		q.y <= math.Max(p.y, r.y) && q.y >= math.Min(p.y, r.y))
} 

// To find orientation of ordered triplet (p, q, r). 
// The function returns following values 
// 0 --> p, q and r are colinear 
// 1 --> Clockwise 
// 2 --> Counterclockwise 
func orientation( p,q,r Point) int { 
	val := (q.y - p.y) * (r.x - q.x) - 
			(q.x - p.x) * (r.y - q.y); 

	if (val == 0) {return 0} // colinear 

	if(val > 0){  
		return 1
	}else{
		 return 2
	} // clock or counterclock wise 
}

// The main function that returns true if line segment 'p1q1' 
// and 'p2q2' intersect. 
func doIntersect( p1, q1, p2, q2 Point) bool{ 
	// Find the four orientations needed for general and 
	// special cases 
	var o1, o2, o3, o4 int
	o1 = orientation(p1, q1, p2); 
	o2 = orientation(p1, q1, q2); 
	o3 = orientation(p2, q2, p1); 
	o4 = orientation(p2, q2, q1); 

	// General case 
	if (o1 != o2 && o3 != o4) {
		return true; 
	}

	// Special Cases 
	// p1, q1 and p2 are colinear and p2 lies on segment p1q1 
	if (o1 == 0 && onSegment(p1, p2, q1)) {return true} 

	// p1, q1 and q2 are colinear and q2 lies on segment p1q1 
	if (o2 == 0 && onSegment(p1, q2, q1)) {return true}

	// p2, q2 and p1 are colinear and p1 lies on segment p2q2 
	if (o3 == 0 && onSegment(p2, p1, q2)) {return true} 

	// p2, q2 and q1 are colinear and q1 lies on segment p2q2 
	if (o4 == 0 && onSegment(p2, q1, q2)) {return true}

	return false // Doesn't fall in any of the above cases 
} 



//------------------


func createPoint() Point{
	return Point { x : rand.Float64()*200.0 - 100 ,
	               y : rand.Float64()*200.0 - 100};
}

func sides(n int) Path{
	var path Path
	for i:=0;i<n;i++ {
		var isReady bool = false
		var intersect bool
		for !isReady {
			var newPoint Point = createPoint();
			var j int=0; 
			for ; j<i-2; j++ {
				//checks intersections from line P(i) - P(i-1) with some line P(j) - P(j+1)
				var intersectLastLine, currentPoint bool
				if (i == n-1 && j!=0) {
					intersectLastLine=doIntersect(newPoint, path[0], path[j],path[(j+1)%n])
					//when i == n-1 this point generates 2 lines, P(n-1) - P(n-2) and  the line P(n-1) - P(0)
					// we have to check here the line P(n-1) - P(0)
				}else{
					intersectLastLine = false
				}
				currentPoint = doIntersect(newPoint, path[i-1], path[j],path[(j+1)%n]) 
				intersect = currentPoint || intersectLastLine
				if(intersect){break;} // repeat process till you have a point.
			}
			if(j>=i-2){ 
				isReady =true
				path=append(path,newPoint);
			}
		}	
	}
	return path
}

func main(){
	rand.Seed(time.Now().UnixNano())

	if len(os.Args) != 2{
		fmt.Printf("Add the number of sides of the figure as argument.\n")
		return
	}
	input := os.Args[1]
	nSide, err := strconv.Atoi(input)
	if err !=nil {
		fmt.Printf("Please, introduce an integer as arguent.\n")
		return
	}

	var path Path = sides(nSide);
	fmt.Printf("-Generating a [%v] sides figure", nSide)
	fmt.Printf("-Figure's vertices\n")
	for i:=0;i<nSide;i++{
		fmt.Printf("  ( %.2f, %.2f),\n", path[i].X(), path[i].Y())
	}
	fmt.Printf("-Figure's Perimeter\n")
	fmt.Printf("-")
	for i:=0;i<nSide;i++{
		if(i==0){
			fmt.Printf("   %.2f",path[i].Distance(path[(i+1)%nSide]))
		}else{
			fmt.Printf(" + %.2f",path[i].Distance(path[(i+1)%nSide]))
		}
		
	}
	fmt.Printf(" = %.2f\n", path.Distance()+ path[nSide-1].Distance(path[0])) // Path.Distance doesn't take in consideration the last line from P(n-1) to P(0)
}