#ifndef __TABLE_H
#define __TABLE_H

#include <string>
#include <cstring>
#include <fstream>
#include <sstream>
#include <tuple>
#include <vector>
#include <iostream>
#include <unordered_map>
using namespace std;
using std::istringstream;
//ask hendrix if attributes that appear more than once in the table need to be accounted for
enum datatype_t { UNKNOWN, BOOL, INT, STRING };

class Table;
class Row;
class Query;

class Table
{
private:
  static unordered_map<string, Table*> tables;
  vector<pair<string, datatype_t> >attr;
  vector<Row> tableObjects;
  string name;

public:
  Table() {}
  
  //sorts attr vector based on input vector
  void sortAttributes(vector<string>& ref);

  void parse(string &line, vector<string> &attr);
  
  //Constructs a Table based on the data in the given CSV file
  //Name of Table should be the string before ".csv"
  static void readTableFromCSV(const string& file);
  

  //Constructs a Table based on the data in the file (name + ".dat")
  static Table* getTableByName(const string& name)
    { return tables.count(name)?  tables[name] : nullptr; }

  //Returns the datatype of the given attribute
  datatype_t getAttributeType(const string& attName) const;
  
  //copy assignment
  const Table& operator=(const Table& copy);

  //Runs the given query on this Table, and returns a new Table with the result
  //The rows of the new Table should be the ones that cause q.getCondition()->getBoolValue(row) to return true
  //The attributes should be the ones returned by q.getAttributesToReturn(), unless
  // q.getAttributesToReturn returns the single attribute "*" (new table has all attributes)
  Table* runQuery(Query& q) const;

  static void printTableNames() 
    { for (auto& p : tables) cout << p.first << '\n'; }

  //modifies input to be used for printing.
  void printTable(vector<vector<string> >& t) const;
  //returns table size
  int tableSize() const { return tableObjects.size(); }

  //Returns this table's name
  string getName() const { return name; }
  //Removes this table from the map when deleted
  ~Table()
    { if (tables.count(name) > 0) tables.erase(name); }
};

class Row
{
private:
  vector<tuple<string, datatype_t, string> > row;
  
public:
  //Creates a dummy row with no data
  Row() {}
  //insert Into row
  Row(vector<tuple<string, datatype_t, string> > row){ this->row = row; }

  //erase by attribute, 1st element in tuple
  void erase(string attr);

  //sort row to match the order of attributes wanted by user
  void sort(vector<string>& ref);

  //prints out data, 3rd element in tuple
  vector<string> printRow();
  
  const void* getValue(const string& attName) const;
};

//Prints the given Table to an output stream
//Format is the same as the CSV file
ostream& operator<<(ostream& out, const Table& t);

// *** Add the line below to table.cpp ***
//unordered_map<string, Table*> Table::tables;
//Initializes static data member

#endif //__TABLE_H