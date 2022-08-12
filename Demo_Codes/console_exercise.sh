#!/bin/bash

function showRun() {
	echo "> $@"
	$@
}

function wait() {
	read -p "Press enter to continue"
}

function doExample() {
	echo -e "Now showing code\n"
	if [ -z "$2" ]
	then
  	vim $1
  else
    diff -w -W 160 --color -y "$2" "$1"
  fi
  wait

	echo -e "\nCompiling...\n"
	showRun g++ -O3 -Wall -Wextra -fopenmp -g -o example $1
	wait

	echo -e "\nRunning...\n"
	export OMP_NUM_THREADS=4
	showRun time -p ./example
	rm example
	wait
}

function doExample9_10() {
	echo -e "Now showing code\n"
	if [ -z "$2" ]
	then
  	vim $1
  else
    diff -w -W 160 --color -y "$2" "$1"
  fi
  wait

	echo -e "\nCompiling...\n"
	showRun g++ -Wall -Wextra -fopenmp -g -o example $1
	wait

	echo -e "\nRunning...\n"
	#Don't foget to adjust this number for your machine
	export OMP_NUM_THREADS=144
	showRun time -p ./example
	rm example
	wait
}

doExample "example_1_traverse_binary_tree_sequential.cpp"
doExample "example_2_traverse_binary_tree_sections.cpp" "example_1_traverse_binary_tree_sequential.cpp"
doExample "example_2_traverse_binary_tree_sections_nested.cpp" "example_2_traverse_binary_tree_sections.cpp"
doExample "example_3_hello_world_task.cpp"
doExample "example_4_hello_world_task_2.cpp" "example_3_hello_world_task.cpp"
doExample "example_5_single_thread_task_creation.cpp" "example_3_hello_world_task.cpp"
doExample "example_6_list_traversal_sequential.cpp"
doExample "example_7_list_traversal.cpp" "example_6_list_traversal_sequential.cpp"
doExample "example_8_fibonacci_sequential.cpp"
doExample9_10 "example_9_fibonacci_task.cpp" "example_8_fibonacci_sequential.cpp"
doExample9_10 "example_10_fibonacci_task_final.cpp" "example_8_fibonacci_sequential.cpp"
