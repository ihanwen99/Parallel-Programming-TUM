all:
	g++ -O3 -Wall -Wextra -fopenmp -g -o example_1 example_1_traverse_binary_tree_sequential.cpp
	g++ -O3 -Wall -Wextra -fopenmp -g -o example_2 example_2_traverse_binary_tree_sections.cpp
	g++ -O3 -Wall -Wextra -fopenmp -g -o example_2_1 example_2_traverse_binary_tree_sections_nested.cpp
	g++ -O3 -Wall -Wextra -fopenmp -g -o example_3 example_3_hello_world_task.cpp
	g++ -O3 -Wall -Wextra -fopenmp -g -o example_4 example_4_hello_world_task_2.cpp
	g++ -O3 -Wall -Wextra -fopenmp -g -o example_5 example_5_single_thread_task_creation.cpp
	g++ -O3 -Wall -Wextra -fopenmp -g -o example_6 example_6_list_traversal_sequential.cpp
	g++ -O3 -Wall -Wextra -fopenmp -g -o example_7 example_7_list_traversal.cpp
	g++  -Wall -Wextra -fopenmp -g -o example_8 example_8_fibonacci_sequential.cpp
	g++  -Wall -Wextra -fopenmp -g -o example_9 example_9_fibonacci_task.cpp
	g++  -Wall -Wextra -fopenmp -g -o example_10 example_10_fibonacci_task_final.cpp

run:
	@echo "running example 1:" && ./example_1
	@echo "running example 2:" && ./example_2
	@echo "running example 2:" && ./example_2_1
	@echo "running example 3:" && ./example_3
	@echo "running example 4:" && ./example_4
	@echo "running example 5:" && ./example_5
	@echo "running example 6:" && ./example_6
	@echo "running example 7:" && ./example_7
	@echo "running example 8:" && ./example_8
	@echo "running example 9:" && ./example_9
	@echo "running example 10:" && ./example_10
clean:
	rm -rf example_1  example_2 example_2_1 example_3 example_4 example_5 example_6 example_7 example_8 example_9 example_10

