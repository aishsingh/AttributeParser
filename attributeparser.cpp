#include <cmath>
#include <cstdio>
#include <vector>
#include <stack>
#include <sstream>
#include <iostream>
#include <algorithm>  // find
#include <cstddef>  // size_t

using namespace std;

class Attribute {
    public:
        string name;
        string value;
};

class Tag {
    public:
        string name;
        vector<Attribute> attributes;
        vector<Tag*> tags;
};


string performQuery(vector<Tag*> *tags, string q) {
    // Interpret query
    string tagName;
    vector<size_t> found;
    found.push_back(0);
    found.push_back(q.find_first_of(".~"));
    while (found.back() != string::npos)
        found.push_back(q.find_first_of(".~", found.back()+1));
    
    // Attrib not specified
    if (q.at(found.at(found.size()-2)) != '~') return "Not Found!";
    
    vector<string> command;
    command.push_back(q.substr(found.at(0), found.at(1)));
    for (int i=1; i<found.size()-1; i++)
        command.push_back(q.substr(found.at(i)+1, found.at(i+1)-found.at(i)-1));
    
    // Test
    /*
    cout << "Test query: ";
    for (int i=0; i<command.size(); i++)
        cout << command.at(i) << " ";
    cout << endl;
    */
    
    // Get response
    for (int i=0; i<command.size(); i++) {
        for (int j=0; j<tags->size(); j++) {
            if (tags->at(j)->name == command.at(i)) {
                //cout << "Match " << command.at(i) << endl;
                if (i < command.size()-2) { // last section of command
                    tags = &tags->at(j)->tags;
                    break;
                }
                else { // command incomplete
                    for (int k=0; k<tags->at(j)->attributes.size(); k++) {
                        if (tags->at(j)->attributes.at(k).name == command.back())
                            return tags->at(j)->attributes.at(k).value;
                    }
                    
                    return "Not Found!";
                    //return "Not Found! " + tags->at(j)->attributes.at(k).name + " != " + command.back();
                }
            }
        }
    }
        
    return "Not Found!";
}

vector<Tag*> parseTags(vector<string> code) {
    vector<Tag*> tags;
    stack<Tag*> tags_stack;
    
    for (int i=0; i<code.size(); i++) {
        stringstream ss(code[i]);
        
        string tagBracket;
        ss >> tagBracket;
        if (tagBracket.find('/') != string::npos) // closing tag
            tags_stack.pop();
        else {  // starting tag
            string name = tagBracket.substr(1, tagBracket.length()-1);
            if (name.at(name.length()-1) == '>')
                name = name.substr(0, name.length()-1);
            Tag *tag = new Tag();
            tag->name = name;
                
            while (ss.good()) {
                Attribute attrib;
                ss >> attrib.name;
                ss.ignore(code[i].length(), '=');
                string value;
                ss >> value;
                value = value.substr(1, value.length()-2);
                if (value.at(value.length()-1) == '"')
                    value = value.substr(0, value.length()-1);
                attrib.value = value;
                
                if (attrib.value.at(attrib.value.length()-1) == '"')
                    attrib.value = attrib.value.substr(0, attrib.value.length()-1);
                
                tag->attributes.push_back(attrib);
                //cout << name << "-> " << attrib.name << ":" << attrib.value << endl;
            }
            
            if (tags_stack.size() == 0) {
                tags.push_back(tag);
                tags_stack.push(tag);
            }
            else {
                tags_stack.top()->tags.push_back(tag);
                tags_stack.push(tag);
            }
        }
    }
    
    // Test
    /*
    cout << "Test code: " << endl;
    for (int i=0; i<tags.size(); i++) {
        cout << tags.at(i)->name << " ";
        
        for (int j=0; j<tags.at(i)->attributes.size(); j++) {
            cout << "attri: " << tags.at(i)->attributes.at(j).name << ":";
            cout << tags.at(i)->attributes.at(j).value << " ";
        }
        cout << endl;
        if (tags.at(i)->tags.size() > 0) {
            for (int j=0; j<tags.at(i)->tags.size(); j++) {
                cout << "  " << tags.at(i)->tags.at(j)->name << " ";
                
                for (int k=0; k<tags.at(i)->tags.at(j)->attributes.size(); k++) {
                    cout << "attri: " << tags.at(i)->tags.at(j)->attributes.at(k).name << ":";
                    cout << tags.at(i)->tags.at(j)->attributes.at(k).value << " ";
                }
            }
        }
    }
    cout << endl;
    */
    
    return tags;
}

int main() {
    int line_count, query_count;
    cin >> line_count >> query_count >> ws;
    vector<string> code(line_count);
    vector<string> querys(query_count);
    for (int i=0; i<line_count; i++)
        getline(cin, code[i]);
    for (int i=0; i<query_count; i++)
        getline(cin, querys[i]);
    
    vector<Tag*> tags = parseTags(code);
    
    for (int i=0; i<query_count; i++)
        cout << performQuery(&tags, querys[i]) << endl;

    return 0;
}
