#include <bits/stdc++.h>
#define pb push_back
#define mp make_pair
using namespace std;

//Integer, either 0 or 1. Used to check whether a subroutine function correctly executed or not. Initially 0
int invalid;

class Table 
{
private:
    int schema_size;                            //size of attribute set
    string name;                                //relation name
    vector<string> schema;                      //table schema
    vector<string> pk_schema;                   //primary key(prime attributes)
    vector<string> rest_schema;                 //non-prime attributes
    /*
      Map from X-->Y where X is the primary key,Y are non-prime attributes. Map
      restricts duplication of two records with same primary key values.
    */

  public:
    Table(string name, vector<string> schema, vector<string> pk_schema);
    int get_schema_size();
    string get_table_name();
    vector<string> get_schema();
    vector<string> get_pk_schema();
    vector<string> get_rest_schema();
    pair<pair<string , vector<string>> , set<vector<string> >> getRecords();
    map<vector<string>,vector<string>> records;
};

string Table :: get_table_name(){               //return table name
    return name;
}

int Table :: get_schema_size(){                 //return size of attributes
    return schema_size;
}
vector<string> Table :: get_schema(){           //return table schema
    return schema;
}
vector<string> Table :: get_pk_schema(){        //return primary key(prime attributes)
    return pk_schema;
}
vector<string> Table :: get_rest_schema(){      //return non-prime attributes
    return rest_schema;
}
Table :: Table(string name, vector<string> schema, vector<string> pk_schema){

    /*Error checking: check if primary key atributes are a subset of schema*/
    bool subset=1;
    string tmp1;
    for(auto v : pk_schema) {
        if(find(schema.begin(),schema.end(),v)==schema.end()){
            subset=0;
            tmp1=v;
            break;
        }
    }
    if(!subset){
        cout << "Attribute "<< tmp1 << " given as primary key is not a part of schema\n";
        invalid=1;                                          //Set invalid 
        return;
    }

    /*Error handling: check if primary key is not null*/
    if(pk_schema.size()==0){
        cout << "Primary key can't be left blank\n";
        invalid=1;                                          //Set invalid
        return;
    }

    this->name = name;
    this->schema = schema;
    this->pk_schema = pk_schema;
    this->schema_size = schema.size();
    vector<string> tmp;

    /*initialize non-prime attrubutes*/
    for(auto s : schema) if (find(pk_schema.begin(),pk_schema.end(),s)==pk_schema.end()) tmp.push_back(s);
    this->rest_schema = tmp;
}

pair<pair<string , vector<string>> , set<vector<string> >> Table :: getRecords()
{
    /*
      This function returns a triplet of relation name, relation schema, set of entries in relation.
    */
    pair<pair<string , vector<string>> , set<vector<string> >> records;
    records.first.first = this->get_table_name();       //relation name
    records.first.second = this->get_schema();          //relation schema
    pk_schema = this->get_pk_schema();                  //primary key
    rest_schema = this->get_rest_schema();              //non-prime attributes
    for(auto entry : this->records){
        /*
          this->records is a map of records 
        */
        vector<string> rec;
        vector<string> key = entry.first;
        vector<string> val = entry.second;
        for(auto attribute : records.first.second) {
            if(find(pk_schema.begin(),pk_schema.end(),attribute)==pk_schema.end()){
                int index = find(rest_schema.begin(),rest_schema.end(),attribute) - rest_schema.begin();
                rec.pb(val[index]);
            }
            else{
                int index = find(pk_schema.begin(),pk_schema.end(),attribute) - pk_schema.begin();
                rec.pb(key[index]);
            }
        }
        records.second.insert(rec);
    }
    return records;
}

map<string, Table*> database;                           //mapping from table name to Table object

/*
  mapping from table name to boolean vale denoting whether table contents have been changed
  by insert/create queries etc. If unchanged=false, then update the file of the associated 
  table in the last.
*/
map<string, bool> unchanged; 

/*
  list of table names newly created in the current session. For all such tables, insert their
  information(table_name, schema, primary key etc.) in "DBMetadata.txt" file.
*/                           
vector<string> new_tables;

//solve is the main function invoked for solving any type of query...
pair<pair<string , vector<string>> , set<vector<string> >> solve(string query);

vector<string> split(string raw_string , char ch)
{
    /*
      Return a list of all the words in the string, using ch as the separator
      (splits on all whitespace if left unspecified).
    */
    vector<string> tokens;
    if(raw_string.length()==0) return tokens;               //Empty string
    stringstream ss(raw_string);
    string i;
    while (getline(ss,i,ch)) tokens.push_back(i);
    return tokens;
}

int isNumber(string s)
{
    /*
      Check whether a given string can also be represented by an integer,
      used for supporting int datatype in the program.
    */
    for(int i=0;i<s.size();i++) if(s[i]<48 or s[i]>57) return 0;
    return 1;
}

int check(string col, string val, string op)
{
    /*
    Check the validity of comparision between 2 string/int values. Subroutine
    function, called by SELECT() function 

    Inputs:
    - string col : LHS operand
    - string val : RHS operand
    - string op : operator

    Returns:
    - 1 if the condition (col op val) is satisfied , else 0

    Example:
    - 30 < 40 returns 1
    - Devang == Deepak returns 0

    Note : If col and val are int in the relation schema , then comparision is made on the integer values
    */


    //type=0 if col and val are string, else 0
    int type=0;
    if(isNumber(col) and isNumber(val)) type=1;

    /*case : string*/
    if(!type){
        if(op=="="){
            if(col!=val) return 0;
        }
        else if(op=="<"){
            if(col>=val) return 0;
        }
        else if(op==">"){
            if(col<=val) return 0;
        }
        else if(op=="<="){
            if(col>val) return 0;
        }
        else if(op==">="){
            if(col<val) return 0;
        }
        else if(op=="<>"){
            if(col==val) return 0;
        }
        else{
            return -1;
        }
    }

    /*case : number*/
    else if(type){
        if(op=="="){
            if(stoi(col)!=stoi(val)) return 0;
        }
        else if(op=="<"){
            if(stoi(col)>=stoi(val)) return 0;
        }
        else if(op==">"){
            if(stoi(col)<=stoi(val)) return 0;
        }
        else if(op=="<="){
            if(stoi(col)>stoi(val)) return 0;
        }
        else if(op==">="){
            if(stoi(col)<stoi(val)) return 0;
        }
        else if(op=="<>"){
            if(stoi(col)==stoi(val)) return 0;
        }
        else{
            return -1;
        }
    }
    return 1;
}

pair<pair<string , vector<string>> , set<vector<string> >> SELECT(string table_name, vector<string> schema, set <vector<string>> data, vector<string> conditions, vector<string> coloumns)
{
    /*
    Iterate through records of the relation to check for conditions, and then
    project attributes specified in columns. Subroutine function, called by select() function.

    Inputs:
    - table_name: relation name
    - schema: relation schema
    - data: relation records
    - conditions: to be satisfied on the selected records
    - coloumns: to be projected after the select operation is performed

    Returns:
    - output: a triplet of table_name, table_schema, and set of records satisfying the conditions
    */

    pair<pair<string , vector<string>> , set<vector<string> >> output;

    output.first.first = table_name;                    // relation name    
    output.first.second = coloumns;                     //relation schema

    /*iterate through relation records*/
    for (auto v : data){

        int broke=0;                                    //specifies whether row should be included
        
        /*iterate through conditions to be satisfied on current records*/
        for (auto p : conditions){

            /*
              parsed_condition is a list of the conditions to be satified. Each component is of the form:
              column1 operator constant
                OR
              column1 operator column2
            */
            vector<string> parsed_condition = split(p,':');
            string col = parsed_condition[0], op = parsed_condition[1], val = parsed_condition[2];
            int index = find(schema.begin(),schema.end(),col) - schema.begin();

            /*Error handling: check if column is present in the schema*/
            if(index>=schema.size()){
                cout << "Attribute " << col << " not present in the schema\n";
                invalid=1;
                return output;
            }

            col = v[index];
            //if RHS of condition is column rather than a constant, change the val
            if(find(schema.begin(),schema.end(),val)!=schema.end()){
                int index = find(schema.begin(),schema.end(),val) - schema.begin();
                val = v[index];
            }

            //apply and check conditions 
            if(check(col,val,op)==-1){
                cout << "Unrecogonized symbol " << op << ", exiting" << endl;
                invalid=1;
                return output;
            }

            if(check(col,val,op)==0) broke=1;
        }
        /*records successfully passes all conditions*/
        if(!broke){
            vector<string> rec;
            for(auto col : coloumns){
                int index = find(schema.begin(),schema.end(),col) - schema.begin();

                /*Error handling: check if column to be projected is present in the schema*/
                if(index>=schema.size()){
                    cout << "Attribute " << col << " not present in the schema\n";
                    invalid=1;
                    return output;
                }

                rec.push_back(v[index]);
            }
            //insert records into output
            output.second.insert(rec);
        }
    }
    return output;
}

pair<pair<string , vector<string>> , set<vector<string> >> create(string query)
{
    /*
      Create a table if it does not exist.. Similar to CREATE command in MySQL.
    */

    pair<pair<string , vector<string>> , set<vector<string> >> garbage;

    /*find table_name from query*/
    int i = query.find('{');
    int i1 = query.rfind('{');
    string name = query.substr(7,i-8);

    //extract schema from create query.
    vector<string> schema = split(query.substr(i+1,i1-i-3) , ',');

    //extract primary key from create query.
    vector<string> pk_schema = split(query.substr(i1+1,query.size()-i1-2) , ',');

    /*check if table already exists*/
    if(database.find(name) != database.end()){
        cout << "Table already exists!!\n";
        invalid=1;
        return garbage;
    }

    //create table object of Table class
    database[name] = new Table(name, schema, pk_schema);
    cout << "Table " + name + " creation successfull.\n";

    /* update unchanged map and new_tables map */
    unchanged[name] = false;
    new_tables.pb(name);

    return garbage;
}

pair<pair<string , vector<string>> , set<vector<string> >> insert(string query)
{
    /*
      Insert a record in the relation if the value of primary key doesn't 
      exists. Similar to INSERT command in MySQL.
    */

    pair<pair<string , vector<string>> , set<vector<string> >> garbage;

    /*extract table name from query*/
    int i = query.find('{');
    string name = query.substr(7,i-8);

    //extract values to be inserted 
    vector<string> vals = split(query.substr(i+1,query.size()-i-2),',');

    /*Error handling: check whether table is present in the database or not*/
    if(database.find(name)==database.end()){
        cout << "Table " << name << " not present in the database\n";
        invalid=1;
        return garbage;
    }

    /*Get table and it's attributes from database<string, Table*> map*/
    Table* table = database[name];
    vector<string> pk_schema = table->get_pk_schema();
    vector<string> schema = table->get_schema();
    int pk_size = pk_schema.size();

    //key denotes value of primary key, value denotes value of non-prime attributes
    vector<string> key , value;
    for(i=0;i<vals.size();i++){
        if(find(pk_schema.begin(),pk_schema.end(),schema[i])==pk_schema.end()) value.pb(vals[i]);
        else key.pb(vals[i]);
    }

    //check for existence of primary key entry
    if(table->records.find(key)==table->records.end()){
        table->records[key] = value;
        cout << "record inserted successfully..\n";
        unchanged[name] = false;
    }

    //primary key entry already exists..
    else cout << "primary key entry already exists!!\n";
    invalid=1;
    return garbage;
}

pair<pair<string , vector<string>> , set<vector<string> >> select(string query)
{
    /*
      Implement the SELECT(sigma) operation of Relational Algebra. Additionally, columns to be
      projected can also be specified in the query(similar to PROJECT(pie) operation of 
      Relational Algebra). Similar to SELECT command in MySQL.
      Iterates through records and finds records satisfying the conditions. Finally projects the
      attributes specified in columns.

      Inputs: 
      - query: relational query

      Returns: 
      - records: a triplet of table_name, table_schema, and set of records
    */

    /*records denotes the full table. If query is nested , recurse in the nested part
    to calculate records, otherwise set it equal to table->getRecords() */
    pair<pair<string , vector<string>> , set<vector<string> >> records;

    /*check if query is nested or not*/
    if(query.find('[')==string::npos){
        //simple query..
        int i = query.find('{');
        string name = query.substr(7,i-8);

        /*Error handling: check whether table is present in the database or not*/
        if(database.find(name)==database.end()){
            cout << "Table " << name << " not present in the database\n";
            invalid=1;
            return records;
        }

        Table* table = database[name];
        records = table->getRecords();
    }
    else{
        //nested query..
        int i = query.find('[');
        int l=0,j;
        for(j=i;j<query.size();j++){
            if(query[j]=='[') l++;
            else if(query[j]==']') l--;
            if(l==0) break;
        }
        //recurse in the nested part for allocating value to records..
        records = solve(query.substr(i+1,j-i-1));
    }

    /*extract columns to be projected in the last(after applying conditions)*/
    int i1 = query.rfind('{');
    vector<string> cols = split(query.substr(i1+1,query.size()-i1-2),',');

    /*extract conditions from the select query*/
    int i = query.substr(0,i1-1).rfind('{');
    vector<string> conditions = split(query.substr(i+1,i1-i-3) , ',');

    return SELECT(records.first.first , records.first.second , records.second , conditions , cols);
}

pair<pair<string , vector<string>> , set<vector<string> >> rename(string query)
{
    /*
      Implementation of RENAME(row) operation of Boolean Algebra. Table as well
      as it's attributes can be renamed.

      Inputs: 
      - query: relational query

      Returns: 
      - records: a triplet of table_name(modified), table_schema(modified), and set of records(unchanged)
    */

    pair<pair<string , vector<string>> , set<vector<string> >> records;

    /*check if query is nested or not*/ 
    if(query.find('[')==string::npos){
        //simple query..
        int i = query.find('{');
        string name = query.substr(7,i-8);

        /*Error handling: check whether table is present in the database or not*/
        if(database.find(name)==database.end()){
            cout << "Table " << name << " not present in the database\n";
            invalid=1;
            return records;
        }

        Table* table = database[name];
        records = table->getRecords();
    }
    else{
        //nested query..
        int i = query.find('[');
        int l=0,j;
        for(j=i;j<query.size();j++){
            if(query[j]=='[') l++;
            else if(query[j]==']') l--;
            if(l==0) break;
        }
        //recurse in the nested part for allocating value to records..
        records = solve(query.substr(i+1,j-i-1));
    }

    //extract the columns(and their new names) to be renamed
    int i1 = query.rfind('{');
    vector<string> cols = split(query.substr(i1+1,query.size()-i1-2),',');
    for(auto attribute_pair : cols){
        vector<string> attributes = split(attribute_pair,':');
        string col1 = attributes[0] , col2 = attributes[1];
        int index = find(records.first.second.begin(), records.first.second.end(), col1) - records.first.second.begin();
        
        //column to be renamed doesn't exist
        if(index==records.first.second.size()){
            cout << "Atrribute " << col1 << " not found, exiting !!";
            invalid=1;
            return records;
        }
        records.first.second[index] = col2;
    }

    /*extract new name of the table*/
    int i2 = query.substr(0,i1).rfind('{');
    int i3 = query.substr(0,i1).rfind('}');
    string new_name = query.substr(i2+1,i3-i2-1);

    //check if table is to be renamed
    if(new_name.size()!=0) records.first.first = new_name;

    return records;
}

pair<pair<string , vector<string>> , set<vector<string> >> project(string query)
{
    /*
      Implementation of PROJECT(pie) operation of Relational Algebra. Implemented via
      the select function(line:XXX) with no conditions.

      Inputs: 
        - query: relational query

      Returns: 
        - records: a triplet of table_name(modified), table_schema(modified), and set of records(unchanged)
    */

    /*extract table_name*/
    int i = query.rfind('{');
    string p1 = query.substr(8,i-8) , p2 = query.substr(i,query.size()-i);

    //prepare modifed query for sending to select function
    string mod_query = "SELECT " + p1 + "{} " + p2;
    
    return select(mod_query); 
}

pair<pair<pair<string , vector<string>> , set<vector<string> >>,pair<pair<string , vector<string>> , set<vector<string> >>> getRecordsfromUnionIntersectDiffCross(string query, string type)
{
    /*
      Compute 2 sets of records from queries involving union/intersection/set difference/cross-product.
      Subroutine function , called from union(),intersect(),difference(),cross(). 

      Inputs:
      - query: relational query
      - type: one of "Union","Intersect","Difference","Cross"

      Returns:
      - pair<records1,records2>: 2 sets of records on which the "type" operation is performed
    */
    pair<pair<string , vector<string>> , set<vector<string> >> records1, records2;

    int type_index = type.size()+1;

    /*nested query, recurse in the nested part for records1*/
    if(query[type_index]=='['){
        int i=type_index,j,l=0;
        for(j=i;j<query.size();j++){
            if(query[j]=='[') l++;
            else if(query[j]==']') l--;
            if(l==0) break;
        }
        records1 = solve(query.substr(i+1,j-i-1));
    }
    /*simple query for records1*/
    else{
        int i=type_index;
        string name = "";
        while(query[i]!=' ') name += query[i] , i++;

        /*Error handling: check whether table is present in the database or not*/
        if(database.find(name)==database.end()){
            cout << "Table " << name << " not present in the database\n";
            invalid=1;
            return mp(records1,records2);
        }

        Table* table = database[name];
        records1 = table->getRecords();
    }
    /*nested query, recurse in the nested part for records2*/
    int len = query.size();
    if(query[len-1]==']'){
        int j=len-1,i,l=0;
        for(i=j;i>=0;i--){
            if(query[i]=='[') l--;
            else if(query[i]==']') l++;
            if(l==0) break;
        }
        records2 = solve(query.substr(i+1,j-i-1));
    }
    /*simple query for records2*/
    else{
        int i = query.rfind(' ');
        string name = query.substr(i+1,len-i-1);

        /*Error handling: check whether table is present in the database or not*/
        if(database.find(name)==database.end()){
            cout << "Table " << name << " not present in the database\n";
            invalid=1;
            return mp(records1,records2);
        }

        Table* table = database[name];
        records2 = table->getRecords();
    }

    return mp(records1,records2);
}

pair<pair<string , vector<string>> , set<vector<string> >> unite(string query)
{
    /*
      Implements the UNION(U) operation of Relational Algebra on two tables. Tables are computed 
      using the getRecordsfromUnionIntersectDiffCross() function.

      Inputs: 
        - query: relational query

      Returns: 
        - output: a triplet of table_name, table_schema, and set of records
    */

    /*Compute 2 tables*/
    pair<pair<string , vector<string>> , set<vector<string> >> records1, records2;
    pair<pair<pair<string , vector<string>> , set<vector<string> >> , pair<pair<string , vector<string>> , set<vector<string> >>> recs = getRecordsfromUnionIntersectDiffCross(query, "Unite");
    records1 = recs.first;
    records2 = recs.second;   

    pair<pair<string , vector<string>> , set<vector<string> >> output;

    /*Check for identical schemas of 2 tables, which is a pre-requisite for union operation.*/ 
    if(records1.first.second!=records2.first.second){
        cout << "Union can't be performed on non-identical schemas.. exiting\n";
        invalid=1;
        return output;
    }

    //set schema for output
    output.first.second = records1.first.second;

    /*
      Set table_name for output. Two cases arise:
      - table1.name = table2.name : output.name = table1.name
      - table1.name != table2.name : output.name = table1.name + table2.name
    */
    output.first.first = records1.first.first;
    if(records1.first.first!=records2.first.first) output.first.first += records2.first.first;

    /*Perform Union on records1 and records2. Since tables are sets, hence duplicacy is automatically removed
    when a common entry is inserted again*/
    for(auto entry : records1.second) output.second.insert(entry);
    for(auto entry : records2.second) output.second.insert(entry);  

    return output;
}

pair<pair<string , vector<string>> , set<vector<string> >> cross(string query)
{
    /*
      Implements the CROSS(X) operation of Relational Algebra on two tables. Tables are computed 
      using the getRecordsfromUnionIntersectDiffCross() function.

      Inputs: 
        - query: relational query

      Returns: 
        - output: a triplet of table_name, table_schema, and set of records
    */
    cout << "Cross starting...\n";

    /*Compute 2 tables*/
    pair<pair<string , vector<string>> , set<vector<string> >> records1, records2;
    pair<pair<pair<string , vector<string>> , set<vector<string> >> , pair<pair<string , vector<string>> , set<vector<string> >>> recs = getRecordsfromUnionIntersectDiffCross(query, "Cross");
    records1 = recs.first;
    records2 = recs.second;    

    pair<pair<string , vector<string>> , set<vector<string> >> output;

    //set table name for output
    output.first.first = records1.first.first + records2.first.first;

    /*set schema for output. For attributes with same name in both tables, rename them to table1.attribute and table2.attribute*/
    for(auto att : records1.first.second){
        if(records1.first.first!=records2.first.first and find(records2.first.second.begin(),records2.first.second.end(),att)!=records2.first.second.end())
            output.first.second.pb(records1.first.first+"."+att);
        else output.first.second.pb(att);
    }
    for(auto att : records2.first.second){
        if(records1.first.first!=records2.first.first and find(records1.first.second.begin(),records1.first.second.end(),att)!=records1.first.second.end())
            output.first.second.pb(records2.first.first+"."+att);
        else output.first.second.pb(att);
    }

    /*Perform Cartesian Product on 2 records*/
    for(auto entry1 : records1.second){
        for(auto entry2 : records2.second){
            vector<string> rec;
            for(auto val : entry1) rec.pb(val);
            for(auto val : entry2) rec.pb(val);
            output.second.insert(rec);
        }
    }

    return output;
}

pair<pair<string , vector<string>> , set<vector<string> >> intersect(string query)
{
    /*
      Implements the INTERSECY(inverted U) operation of Relational Algebra on two tables. Tables are computed 
      using the getRecordsfromUnionIntersectDiffCross() function.

      Inputs: 
        - query: relational query

      Returns: 
        - output: a triplet of table_name, table_schema, and set of records
    */

    /*Compute 2 tables*/
    pair<pair<string , vector<string>> , set<vector<string> >> records1, records2;
    pair<pair<pair<string , vector<string>> , set<vector<string> >>,pair<pair<string , vector<string>> , set<vector<string> >>> recs = getRecordsfromUnionIntersectDiffCross(query, "Intersect");
    records1 = recs.first;
    records2 = recs.second;    

    pair<pair<string , vector<string>> , set<vector<string> >> output;

    /*Check for identical schemas of 2 tables, which is a pre-requisite for intersection operation.*/ 
    if(records1.first.second!=records2.first.second){
        cout << "Intersection can't be performed on non-identical schemas.. exiting\n";
        invalid=1;
        return output;
    }

    //set schema for output
    output.first.second = records1.first.second;

    /*
      Set table_name for output. Two cases arise:
      - table1.name = table2.name : output.name = table1.name
      - table1.name != table2.name : output.name = table1.name + table2.name
    */
    output.first.first = records1.first.first;
    if(records1.first.first!=records2.first.first) output.first.first += records2.first.first;
    output.first.second = records1.first.second;

    /*Perform Intersection on 2 tables*/
    for(auto entry : records1.second){
        if(records2.second.find(entry) != records2.second.end()) output.second.insert(entry);
    }

    return output;
}

pair<pair<string , vector<string>> , set<vector<string> >> difference(string query)
{
    /*
      Implements the DIFFERENCE (- ) operation of Relational Algebra on two tables. Tables are computed 
      using the getRecordsfromUnionIntersectDiffCross() function.

      Inputs: 
        - query: relational query

      Returns: 
        - output: a triplet of table_name, table_schema, and set of records
    */

    /*Compute 2 tables*/
    pair<pair<string , vector<string>> , set<vector<string> >> records1, records2;
    pair<pair<pair<string , vector<string>> , set<vector<string> >>,pair<pair<string , vector<string>> , set<vector<string> >>> recs = getRecordsfromUnionIntersectDiffCross(query, "Diff");
    records1 = recs.first;
    records2 = recs.second; 

    pair<pair<string , vector<string>> , set<vector<string> >> output;

    /*Check for identical schemas of 2 tables, which is a pre-requisite for set-difference operation.*/    
    if(records1.first.second!=records2.first.second){
        cout << "Difference can't be performed on non-identical schemas.. exiting\n";
        invalid=1;
        return output;
    }

    //set schema for output
    output.first.second = records1.first.second;

    /*
      Set table_name for output. Two cases arise:
      - table1.name = table2.name : output.name = table1.name
      - table1.name != table2.name : output.name = table1.name + table2.name
    */
    output.first.first = records1.first.first;
    if(records1.first.first!=records2.first.first) output.first.first += records2.first.first;
    output.first = records1.first;

    /*Perform set difference on 2 tables*/
    for(auto entry : records1.second){
        if(records2.second.find(entry) == records2.second.end()) output.second.insert(entry);
    }
    
    return output;
}

pair<pair<string , vector<string>> , set<vector<string> >> solve(string query)
{
    /*
      Primary function, called from main()

      Inputs: 
        - query: relational query

      Returns: 
        - output: a triplet of table_name, table_schema, and set of records
    */

    pair<pair<string , vector<string>> , set<vector<string> >> garbage;
    if(query.substr(0,6)=="CREATE") return create(query);
    else if(query.substr(0,6)=="INSERT") return insert(query);
    else if(query.substr(0,6)=="SELECT") return select(query);
    else if(query.substr(0,7)=="PROJECT") return project(query);
    else if(query.substr(0,5)=="UNION") return unite(query);
    else if(query.substr(0,4)=="DIFF") return difference(query);
    else if(query.substr(0,9)=="INTERSECT") return intersect(query);
    else if(query.substr(0,6)=="RENAME") return rename(query);
    else if(query.substr(0,5)=="CROSS") return cross(query);
    else return garbage;
}
const char width=20;
const char seperator = ' ';

template<typename T> void printElement(T t, const int& width)
{
    cout << left << setw(width) << setfill(seperator) << t;
}
void pretty_print(pair<pair<string , vector<string>> , set<vector<string> >> output)
{
    if(output.second.size()==0){
        return;
    }
    //Print the contents of the table
    cout << "table name: " << output.first.first << endl;
    for(auto v : output.first.second) printElement(v,width);
    cout << "\n";
    for(auto v : output.second){
        for(auto x : v) printElement(x,width);
        cout << "\n";
    }
}
string join(vector<string> A,char ch)
{
    /*Returns a string in which the string elements of sequence have been joined by ch separator.*/

    string ans="";
    for(int i=0;i<A.size();i++)
    {
        ans += A[i];
        if(i!=A.size()-1) ans += ch;
    }
    return ans;
}

void init_unchanged_map()
{
    //Initialize unchanged map
    for(auto entry : database) unchanged[entry.first] = true;
}
void init_database_map()
{
    /*Constructs table objects from the table info given in "DBMetadata.txt" and individual table files*/

    //Open metadata file. File consists of table_name, table_schema, primary_key_schema seperated by tab.
    ifstream infile("DBMetadata.txt");

    /*iterate through tables*/
    while (infile){
        string s;
        if (!getline(infile, s)) break;
        istringstream ss(s);

        /*Fill schema_table. schema_table is a vector of table_name, table_schema, primary_key_schema*/
        vector <string> schema_table;
        while (ss){
            string s;
            if (!getline( ss, s, '\t' )) break;
            schema_table.pb(s);
        } 

        /*Extract table_name, table_schema, primary_key_schema*/  
        string name = schema_table[0];
        vector<string> schema = split(schema_table[1],',');
        vector<string> pk_schema = split(schema_table[2],',');

        //Construct table object of Table class
        database[name] = new Table(name, schema, pk_schema);

        //Open data file of table
        ifstream infile(name + ".txt");

        //iterate through table records
        while (infile){
            string s;
            if (!getline(infile, s)) break;
            istringstream ss(s);

            //record is a single entry of the currrent table
            vector <string> record;
            while (ss){
                string s;
                if (!getline( ss, s, '\t' )) break;
                record.push_back(s);
            }
            string rec = join(record,',');
            
            //Insert the record into the current table.
            string query = "INSERT " + name + " {" + rec + '}';
            insert(query);
        }

        //Error handling
        if (!infile.eof()) {
            cout << "No such table exists, exiting\n";
            return;
        }
    }
}

void init()
{
    /*Initializes maps and table objects by calling 2 subroutine functions*/

    init_database_map();
    init_unchanged_map();
}

void write_changed_tables_to_file()
{
    /*
      Write tables back to file systems. Function called at exit time. Table is written in file in 2 cases-:
      - Case1: Create table called. Newly created table is written to new file
      - Case2: Insert into table called. Modified table is re-written to corresponding file
    */

    /*Iterate over modified tables and new tables*/  
    for(auto entry : unchanged) if(unchanged[entry.first]==false){
        
        Table* table = database[entry.first];
        pair<pair<string , vector<string>> , set<vector<string> >> records = table->getRecords();

        //Write table to file
        ofstream myfile (table->get_table_name() + ".txt");
        if (myfile.is_open()){
            for(auto rec : records.second){
                myfile << join(rec,'\t')+"\n";
            }
            myfile.close();
        }

        //Error handling
        else cout << "Unable to open file";
    }

    //Open file "DBMetadata.txt"
    ofstream myfile ("DBMetadata.txt", ofstream::app);
    if (myfile.is_open()){

        /*Iterate over new_tables, append their metadata(table_name,table_schema,primary_key_schema) at the end of "DBMetadata.txt" file*/
        for(auto name : new_tables){
            Table* table = database[name];
            string schema = join(table->get_schema(),',');
            string pk_schema = join(table->get_pk_schema(),',');
            myfile << name + "\t" + schema + "\t" + pk_schema + "\n";
        }
        myfile.close();
    }
}

int main()
{ 
    init();
    cout << "Enter queries :\n";
    string query;
    while((getline(cin,query))){
        if(query=="exit()") break;
        if(query=="show tables"){
            for(auto x : database) cout<<x.first<<endl;
            continue;
        }
        pair<pair<string , vector<string>> , set<vector<string> >> output = solve(query);
        if(invalid){
            invalid=0;
            continue;
        }
        pretty_print(output);
    }
    write_changed_tables_to_file();
    cout<<"bye\n";
    return 0;
} 