#include "query.h"
#include <iostream>
#include <fstream>
using namespace std;

int main()
{
  string cmd;
  ifstream in;
  cin >> cmd;
  while (cmd != "QUIT")
  {
    if (cmd == "READ")
    {
      cin >> cmd;
      in.open(cmd);
      if (in.is_open())
      {
        in.close();
        
        try
        {
          Table::readTableFromCSV(cmd);
          cout << "Success\n";
        }
        catch (...)
        {
          cout << "CSV file " << cmd << " could not be processed" << endl;
        }
      }
      else
        cout << "Could not open file " << cmd << endl;
    }
    else if (cmd == "LIST")
      Table::printTableNames();
    else if (cmd == "PRINT")
    {
      cin >> cmd;
      Table* t = Table::getTableByName(cmd);
      if (t != nullptr)
        cout << *t << endl;
    }
    else if (cmd == "SELECT")
    {
      string line;
      getline(cin, line);
      cmd += line;
      Query q(cmd);
      if (q.getCondition() == nullptr)
        cout << "Error in parsing query \"" << cmd << '"' << endl;
      Table* t = q.getTable();
      
      if (t == nullptr)
        cout << "Table " << q.getTableName() << " does not exist" << endl;
      else
      {
        //cout << "Read table:\n" << *t << endl;
        //cout << "Query:  " << q << endl;
        Table* result = t->runQuery(q);
        cout << *result << endl;
        delete result;
      }
    }
    cin >> cmd;
  }

  return 0;
}