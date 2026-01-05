# 컴파일러 및 플래그 설정
CXX = g++
CXXFLAGS = -I../include $(shell root-config --cflags) -Wall -std=c++20

# 라이브러리 경로 및 라이브러리 파일 설정
LIB_DIR = ../lib
LIBS = -L$(LIB_DIR) -lmylibrary  # 'mylibrary'는 사용하는 .so 파일의 이름입니다.
LIBS += $(shell root-config --libs)  # ROOT 라이브러리

# 소스 파일 및 객체 파일 설정
SRC_DIR = ../src
SRC_FILES = $(SRC_DIR)/*.cc

ARG := default_value
# 테스트 파일 설정
TEST_SRC = $(ARG).cc
TEST_OBJ = $(ARG).o
EXEC = $(ARG)_program  # 실행 파일 이름

# 기본 타겟
all: $(EXEC)

# 테스트 파일 컴파일
$(TEST_OBJ): $(TEST_SRC)
	$(CXX) $(CXXFLAGS) -c $(TEST_SRC) -o $(TEST_OBJ)

# 실행 파일 컴파일 및 링킹
$(EXEC): $(OBJ_FILES) $(TEST_OBJ)
	$(CXX) $(CXXFLAGS) $(OBJ_FILES) $(TEST_OBJ) -o $(EXEC) $(LIBS)

# 객체 파일 컴파일
$(OBJ_FILES): $(SRC_FILES)
	$(CXX) $(CXXFLAGS) -c $(SRC_FILES)

# 청소 타겟
clean:
	rm -f $(OBJ_FILES) $(TEST_OBJ) $(EXEC)
		
