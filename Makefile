# Compilateur
CXX = g++

# Options de compilation
CXXFLAGS = -std=c++17 -Wall -Wextra -pedantic -g

# Nom de l'exécutable
TARGET = myql

# Tous les fichiers source
SRC = main.cpp \
      core/Observer.cpp \
      core/Observable.cpp \
	  core/LazyObject.cpp \
	  instruments/Instrument.cpp \
	  instruments/EQA.cpp \
	  instruments/ELE.cpp \
	  pricingEngines/EQAMonteCarloEngine.cpp \
	  pricingEngines/ELEMonteCarloEngine.cpp \
	  processes/BlackScholesProcess.cpp \
	  processes/DeterministicVolRateProcess.cpp \

	  
# Fichiers objets
OBJ = $(SRC:.cpp=.o)

# Règle principale
all: $(TARGET)

# Link
$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) $(OBJ) -o $(TARGET)

# Compilation des .cpp en .o
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Nettoyage
clean:
	rm -f $(OBJ) $(TARGET)

# Recompilation complète
rebuild: clean all

.PHONY: all clean rebuild




































