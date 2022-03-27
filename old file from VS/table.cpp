//Muhammed Ademola

#include "query.h"
#include "table.h"

unordered_map<string, Table*> Table::tables;
//Initializes static data member


void Table::sortAttributes(vector<string>& ref){
  for(int i = 0; i < ref.size(); i++){
    for(int j = 0; j < attr.size(); j++){
      if(ref[i] == attr[j].first){
        attr[i].swap(attr[j]);
      }
    }
  }
}

void Table::parse(string &line, vector<string> &tuple){
    int i = 0, index = 0;
    string temp = "";
    tuple.clear();
    while(line != temp){
        index = line.find(',');
        if(index == string::npos){
          line.pop_back();//removing /r character, will not work unless using g++(does not work with Visual Studio MSVC)
          temp = line;
        }else{
          temp = line.substr(i, index);
          line = line.substr(index + 1);
        }
        
        //cout << temp << endl << "line: " + line << endl;
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
    //unordered_map<string, pair<datatype_t, string> > tuple;

    int i = 0;

    if(!csvFile){
        cout << "file could not be opened" << endl;
        return; //how to return nullptr? ask Hendrix
    }
    
    //first line in array
    getline(csvFile, line);//csvFile >> line;
    //cout << line << endl;
    table->parse(line, attrName);
    for(auto p : attrName){
      cout << p << ", ";
    }
    
    //second line into array
    getline(csvFile, line);//csvFile >> line;
    //cout << endl << line << endl;
    table->parse(line, attrType);
    for(auto p : attrType){
      cout << p << ", ";
    }


    cout << endl; 
    for(auto p : attrType){
        if(p == "BOOL"){
            table->attr.push_back({attrName[i], BOOL});
        }
        else if(p == "INT"){
            table->attr.push_back({attrName[i], INT});
        }
        else if(p == "STRING"){
            table->attr.push_back({attrName[i], STRING});
        }
        else{
            table->attr.push_back({attrName[i], UNKNOWN});
        }
        i++;
    }
    //getline(csvFile, line);
    while(!csvFile.eof()){
      i = 0;
      getline(csvFile, line, '\n'); //getline(csvFile, line, '\n'); csvFile >> line;
      if(line != ""){
        table->parse(line, dataRow);
        
        for (int i = 0; i < attrName.size() && i < dataRow.size(); i++) {
          if(attrName[i] == "BOOL"){
              tuple.push_back({attrName[i], BOOL, dataRow[i]});
          }
          else if(attrName[i] == "INT"){
              tuple.push_back({attrName[i], INT, dataRow[i]});
          }
          else if(attrName[i] == "STRING"){
              tuple.push_back({attrName[i], STRING, dataRow[i]});
          }
          else{
              tuple.push_back({attrName[i], UNKNOWN, dataRow[i]});
          }
        }
        table->tableObjects.push_back(Row(tuple));//do I need to create a new row object each time since I am inserting into vector? ask hendrix
        tuple.clear();
      }
    }

    
}

datatype_t Table::getAttributeType(const string& attName) const{
    for(auto p : attr){
      if(p.first == attName){
        return p.second;
      }
      else{
        return UNKNOWN;
      }
    }
    return UNKNOWN;
  }

const Table& Table::operator=(const Table &copy)
{
    name = copy.name + "1";
    tableObjects = copy.tableObjects;
    attr = copy.attr;
    return copy;
}

Table* Table::runQuery(Query& q) const{
    Table* cur = new Table();
    cur = q.getTable(); //do we have to get table?

    vector<string> att = q.getAttributesToReturn();//get attributes user wants
    
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
      }//might need an else
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
        for(auto s = cur->tableObjects.begin(); s != cur->tableObjects.end(); s++){
          s->erase(p.first);
        }
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
      
    
    //removing rows from table that aren't needed and sorting the ones that are
    for(auto s = cur->tableObjects.begin(); s != cur->tableObjects.end(); ){
      if(!(q.getCondition()->getBoolValue(*s))){
        s = cur->tableObjects.erase(s);
      }
      else{ 
        s->sort(att);
        s++;
      } 
    }
    

    return cur;
  }


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
        if(get<0>(*i) == attr){//need to look at what *i is going to do here
            row.erase(i);
            break;
        }else{
            i++;
        }
    }
}

void Row::sort(vector<string>& ref){
    for(int i = 0; i < ref.size(); i++){
      for(int j = i; j < row.size(); j++){
        if(ref[i] == get<0>(row[j])){
          if(i == j)
            break;
          //swap(row[row.size() - 1], row[j]);
          row[i].swap(row[j]);
          break;
        }
      }
    }
  }

vector<string> Row::printRow(){
    vector<string> temp;
    for(auto p : row){
      temp.push_back(get<2>(p));
    }
    return temp;
  }

const void* Row::getValue(const string& attName) const{//need to fix to return correct pointers
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

        //return get<2>(p) == "TRUE" ? *val = true : *val = false;
      }
      else if (get<1>(p) == INT){
        int32_t* val = new int32_t;
        *val = stoi(get<2>(p));
        return val;
      }
      else if (get<1>(p) == STRING){
        const char* val = get<2>(p).c_str();
        return val;
      }
      else{
        return nullptr;
      }
    }
  }
  return nullptr;
}

ostream& operator<<(ostream& out, const Table& t){//where do we implement this? Can we declare this a friend of the Table class?
  out << t.getName() << endl << endl;
  
  vector<vector<string> > print;
  t.printTable(print);
  for (int i = 0; i < print.size(); i++){
    for (int j = 0; j < print[i].size(); j++){
      if(j == print[i].size() - 1)
        out << print[i][j] << endl;
      else
        out << print[i][j] << ", ";
    }
  }

  out << endl << "Number of Rows: " << t.tableSize(); 
  return out;
}