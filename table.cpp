//Muhammed Ademola

#include "query.h"
#include "table.h"

//Initializes static data member
unordered_map<string, Table*> Table::tables;


void Table::sortAttributes(vector<string>& ref){
  for(long unsigned int i = 0; i < ref.size(); i++)
    for(long unsigned int j = 0; j < attr.size(); j++)
      if(ref[i] == attr[j].first && i != j)
        attr[i].swap(attr[j]);
}

void Table::parse(string &line, vector<string> &tuple){
    string temp = "";
    tuple.clear();
    istringstream iss(line);
    while(getline(iss, temp, ',')){
      if (temp.find('\r') != string::npos) {//getting rid of /r character in stream 
        temp = temp.substr(0, temp.find('\r'));
      }
      tuple.push_back(temp);
    }    
}

void Table::readTableFromCSV(const string& file){
    ifstream csvFile(file);
    string tableName = file.substr(0, file.find('.'));
    Table* table = new Table();
    tables.insert({tableName, table});
    table->name = tableName;
    
    vector<string> attrName, attrType, dataRow;
    string line = "";
    vector<tuple<string, datatype_t, string> > tuple;

    int i = 0;

    if(!csvFile){
        cout << "file could not be opened" << endl;
        return;
    }
    
    //first line in array
    getline(csvFile, line, '\n');
    table->parse(line, attrName);

    //second line into array
    getline(csvFile, line, '\n');
    table->parse(line, attrType);

    for (long unsigned int i = 0; i < attrType.size() && i < attrName.size(); i++) {
        if (attrType[i] == "BOOL")
            table->attr.push_back({ attrName[i], BOOL });
        else if (attrType[i] == "INT")
            table->attr.push_back({ attrName[i], INT });
        else if (attrType[i] == "STRING")
            table->attr.push_back({ attrName[i], STRING });
        else
            table->attr.push_back({ attrName[i], UNKNOWN });
    }
    
    //matching attribute name to type
    /*for (auto p : attrType) {
        if(p == "BOOL")
            table->attr.push_back({attrName[i], BOOL});
        else if(p == "INT")
            table->attr.push_back({attrName[i], INT});
        else if(p == "STRING")
            table->attr.push_back({attrName[i], STRING});
        else
            table->attr.push_back({attrName[i], UNKNOWN});
        i++;
    }*/
    
    //creating entry tuples to make rows with
    while(!csvFile.eof()){
      i = 0;
      getline(csvFile, line, '\n'); 
      if(line != ""){
        table->parse(line, dataRow);
        
        for (long unsigned int i = 0; i < attrName.size() && i < dataRow.size() && i < attrType.size(); i++){
          if(attrType[i] == "BOOL")
              tuple.push_back({attrName[i], BOOL, dataRow[i]});
          else if(attrType[i] == "INT")
              tuple.push_back({attrName[i], INT, dataRow[i]});
          else if(attrType[i] == "STRING")
              tuple.push_back({attrName[i], STRING, dataRow[i]});
          else
              tuple.push_back({attrName[i], UNKNOWN, dataRow[i]});
        }
        table->tableObjects.push_back(Row(tuple));
        tuple.clear();
      }
    }
}

datatype_t Table::getAttributeType(const string& attName) const{
    for(auto p : attr)
      if(p.first == attName)
        return p.second;
    
    return UNKNOWN;
 }

const Table& Table::operator=(const Table& copy)
{
    name = copy.name + "1";
    tableObjects = copy.tableObjects;
    attr = copy.attr;
    return copy;
}

Table* Table::runQuery(Query& q) const{
    Table* cur = new Table();
    *cur = *q.getTable(); 

    vector<string> att = q.getAttributesToReturn();//get attributes user wants

    //removing rows from table that aren't needed and sorting the ones that are
    for (auto s = cur->tableObjects.begin(); s != cur->tableObjects.end(); ) {
      if(!(q.getCondition()->getBoolValue(*s))){
        s = cur->tableObjects.erase(s);
      }
      else{
        s++;
      } 
    }

    //dealing with columns (attributes)
    if(att.size() == 1){
      if(att[0] != "*"){
        for(auto p : cur->attr){
          if(p.first != att[0]){
            for(auto s = cur->tableObjects.begin(); s != cur->tableObjects.end(); s++){
              s->erase(p.first);
            }
          }
        }

        for (auto t = cur->attr.begin(); t != cur->attr.end(); ) {//erasing irrelevant attributes
          if ((*t).first != att[0]) {
            t = cur->attr.erase(t);
          }
          else {
            t++;
          }
        }
      }
    }
    else{
      auto temp = cur->attr;
      for(auto p : att){//create temp attribute table and erase all attributes that are selected in att in that table
        for(auto t = temp.begin(); t != temp.end(); ){
          if(t->first == p){//erases only first attrbiute that matches
            t = temp.erase(t);
            break;
          }
          else
            t++;
        }
      }

      for(auto p : temp){//use temp table to delete attributes not needed in Row objects
        for (auto s = cur->tableObjects.begin(); s != cur->tableObjects.end(); s++) {
          s->erase(p.first);
        }
      }
      for (auto s = cur->tableObjects.begin(); s != cur->tableObjects.end(); s++) {//sorting row object tuples
          s->sort(att); 
      }
      for(auto p : temp){//also use temp to delete attributes not needed in attribute table
        for(auto s = cur->attr.begin(); s != cur->attr.end(); ){
          if(s->first == p.first){
            s = cur->attr.erase(s);
          }
          else
            s++;
        }
      }
      cur->sortAttributes(att);//sorting all leftover attributes
    }
    return cur;
}

//returns vector to print everything in table
void Table::printTable(vector<vector<string> >& t) const{
  vector<string> r1, r2;
  for(auto p : attr){
    r1.push_back(p.first);
  }

  for(auto p : attr){
    switch(p.second){
      case UNKNOWN: 
        r2.push_back("UNKNOWN");
        break;
      case BOOL: 
        r2.push_back("BOOL");
        break;
      case INT: 
        r2.push_back("INT");
        break;
      case STRING: 
        r2.push_back("STRING");
        break;
    }
  }
  cout << endl;
  t.push_back(r1);
  t.push_back(r2);
  for(auto p : tableObjects)
    t.push_back(p.printRow());
}

//find first attribute with that name and erases it
void Row::erase(string attr){
  for(auto i = row.begin(); i != row.end();){
    if(get<0>(*i) == attr){
      row.erase(i);
      break;
    }else{
      i++;
    }
  }
}
//sort the tuple in row same as vector
void Row::sort(vector<string>& ref){
  for(long unsigned int i = 0; i < ref.size(); i++){
    for(long unsigned int j = i; j < row.size(); j++){
      if(ref[i] == get<0>(row[j]) && i != j){
        row[i].swap(row[j]);
        break;
      }
    }
  }
}
//returns vector to have row printed
vector<string> Row::printRow(){
  vector<string> temp;
  for(auto p : row)
    temp.push_back(get<2>(p));
  
  return temp;
}

const void* Row::getValue(const string& attName) const{
  for(auto p : row){
    if(get<0>(p) == attName){
      if(get<1>(p) == BOOL){
        bool* val = new bool;
        if(get<2>(p) == "FALSE"){
          *val = false;
          return val;
        }else{
          *val = true;
          return val;
        }
      }
      else if (get<1>(p) == INT){
        int32_t* val = new int32_t;
        *val = stoi(get<2>(p));
        return val;
      }
      else if (get<1>(p) == STRING) {
        char* val = new char[get<2>(p).size() + 1];
        memcpy(val, get<2>(p).c_str(), get<2>(p).size() + 1);
        return val;
      }
      else{
        return nullptr;
      }
    }
  }
  return nullptr;
}

ostream& operator<<(ostream& out, const Table& t){
  
  vector<vector<string> > print;
  t.printTable(print);
  for (long unsigned int i = 0; i < print.size(); i++){
    for (long unsigned int j = 0; j < print[i].size(); j++){
      if(j == print[i].size() - 1)
        out << print[i][j] << endl;
      else
        out << print[i][j] << ", ";
    }
  }

  out << endl << "Number of Rows: " << t.tableSize(); 
  return out;
}