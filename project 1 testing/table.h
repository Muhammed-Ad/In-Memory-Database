#ifndef __TABLE_H
#define __TABLE_H

#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include <unordered_map>
using namespace std;

enum datatype_t { UNKNOWN, BOOL, INT, STRING };

class Table;
class Row;
class Query;

class Table
{
private:
  static unordered_map<string, Table*> tables;
  unordered_map<string, datatype_t> attr;
  vector<Row> tableObjects;
  string name;
public:
  //Constructs a Table based on the data in the given CSV file
  //Name of Table should be the string before ".csv"
  void parse(string &line, vector<string> &tuple){
    int i = 0, index = 0;
    string temp = "";
    tuple.clear();
    while(line != temp){
        index = line.find(',');
        temp = line.substr(i, index);
        line = line.substr(index + 1);
        //cout << temp << endl << "line: " + line << endl;
        tuple.push_back(temp);
    }
  }
  
  static void readTableFromCSV(const string& file){
    ifstream csvFile(file);
    string tableName = file.substr(0, file.find('.'));
    Table* table = new Table();
    tables.insert({tableName, table});
    table->name = tableName;
    
    vector<string> attrName, attrType, dataRow;
    string line = "";
    unordered_map<string, pair<datatype_t, string> > tuple;

    int i = 0;

    if(!csvFile){
        cout << "file could not be opened" << endl;
        return; //how to return nullptr? ask Hendrix
    }
    
    //first line in array
    csvFile >> line;
    cout << line << endl;
    table->parse(line, attrName);
        
    //second line into array
    /*csvFile >> line;
    cout << endl << line << endl;
    table->parse(line, attrType);
    for(auto p : attrType){
        if(p == "BOOL"){
            table->attr.insert({attrName[i], BOOL});
        }
        else if(p == "INT"){
            table->attr.insert({attrName[i], INT});
        }
        else if(p == "STRING"){
            table->attr.insert({attrName[i], STRING});
        }
        else{
            table->attr.insert({attrName[i], UNKNOWN});
        }
        i++;
    }
    
    //generic data entries in table
    while(!csvFile.eof()){
        i = 0;
        getline(csvFile, line);
        table->parse(line, dataRow);
        
        for(auto p : attrType){
            if(p == "BOOL"){
                tuple.insert({attrName[i], {BOOL, dataRow[i]}});
            }
            else if(p == "INT"){
                tuple.insert({attrName[i], {INT, dataRow[i]}});
            }
            else if(p == "STRING"){
                tuple.insert({attrName[i], {STRING, dataRow[i]}});
            }
            else{
                tuple.insert({attrName[i], {UNKNOWN, dataRow[i]}});
            }
            i++;
        }
        //table->tableObjects.push_back(Row(tuple));//do I need to create a new row object each time since I am inserting into vector? ask hendrix
    
        tuple.clear();
    }*/
  }

  //Constructs a Table based on the data in the file (name + ".dat")
  static Table* getTableByName(const string& name)
    { return tables.count(name)?  tables[name] : nullptr; }

  //Returns the datatype of the given attribute
  datatype_t getAttributeType(const string& attName) const;

  //Runs the given query on this Table, and returns a new Table with the result
  //The rows of the new Table should be the ones that cause q.getCondition()->getBoolValue(row) to return true
  //The attributes should be the ones returned by q.getAttributesToReturn(), unless
  // q.getAttributesToReturn returns the single attribute "*" (new table has all attributes)
  Table* runQuery(Query& q) const;

  static void printTableNames()
    { for (auto& p : tables) cout << p.first << '\n'; }
  //Returns this table's name
  string getName() const
    { return name; }
  //Removes this table from the map when deleted
  ~Table()
    { if (tables.count(name) > 0) tables.erase(name); }
};

class Row
{
public:
  //Creates a dummy row with no data
  Row() {}

  //Returns the value of the given attribute for this row
  const void* getValue(const string& attName) const;
};

//Prints the given Table to an output stream
//Format is the same as the CSV file
ostream& operator<<(ostream&, const Table&);

// *** Add the line below to table.cpp ***
//unordered_map<string, Table*> Table::tables;
//Initializes static data member

#endif //__TABLE_H