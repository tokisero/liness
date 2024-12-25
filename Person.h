

#include "User.h"
#include "FlightManager.h"

class Person : public User, public FlightManager{
public:
    Person(const std::string& username);
    void menu() override; 
};

