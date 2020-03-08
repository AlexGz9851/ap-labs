package main

import "golang.org/x/tour/pic"

func Pic(dx, dy int) [][]uint8 {
	matrix := make([][]uint8, dx)
	for i:=0;i<dx;i++{
		matrix[i] = make([]uint8, dy)
		for j:=0;j<dy;j++{
			matrix[i][j] = uint8(i*i+j*j)
		}
	}
	return matrix
}

func main() {
	pic.Show(Pic)
}