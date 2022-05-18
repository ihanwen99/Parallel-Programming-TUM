CXX = c++
# CXX_FLAGS = --std=c++17 -Wall -Wextra -march=native -g
CXX_FLAGS = --std=c++17 -Wall -Wextra -g

all: env_file sequential_implementation student_submission reference_implementation

sequential_implementation: Utility.h sequential_implementation.cpp
	$(CXX) $(CXX_FLAGS) -o sequential_implementation sequential_implementation.cpp 

student_submission: Utility.h student_submission.cpp 
	$(CXX) $(CXX_FLAGS) -o student_submission student_submission.cpp

reference_implementation: Utility.h reference_implementation.cpp 
	$(CXX) $(CXX_FLAGS) -o reference_implementation reference_implementation.cpp 

env_file: student_submission.cpp
	#grep -o -P -e '!submission_env \K.*' student_submission.cpp > student_submission.env; [ $$? -lt 2 ]
	sed -n -e 's/.*!submission_env \(.*\)/\1/p' student_submission.cpp > student_submission.env

clean:
	rm -f sequential_implementation student_submission reference_implementation student_submission.env *.o