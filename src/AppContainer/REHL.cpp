#include "AppContainer/REHL.hpp"

namespace App {
using std::cout;
using std::cin;
using std::endl;
const char* PS1 = "KDB>";
const char* PS2 = "....";
void REHL::run() {
    onRun = true;
    while (onRun)
    {
        cout<<PS1;
        ss.clear();
        while(1) {
            if(!getline(cin, line) || line.find_first_not_of(' ') == string::npos) {
                break;
            } else {
                ss << line << endl;
                if(line.find(';') != string::npos) 
                    break;
                else 
                    cout << PS2;
            }
        }
        ss << endl;
        try {
            Parser parser(ss);
            auto statements = parser.parse();
            for(auto statement: statements) {
                try{
                    statement->call();
                } catch (AST::ASTException& e) {
                    switch (e) {
                        case AST::ASTException::SIGQUIT: {
                            onRun = false;
                            break;
                        }
                        case AST::ASTException::SIGSEL: {
                            auto select = dynamic_cast<AST::SelectStatement*>(statement.get());
                            cout << select->callT() << endl;
                            break;
                        }
                        case AST::ASTException::SIGEXEC: {
                            auto exec = dynamic_cast<AST::ExecfileStatement*>(statement.get());
                            auto str = exec->callT();
                            stringstream fss(str);
                            Parser fparser(fss);
                            auto fstatements = fparser.parse();
                            //-------------file begin
                            for(auto fstatement: fstatements) {
                                try{
                                    fstatement->call();
                                } catch (AST::ASTException& e) {
                                    switch (e) {
                                        case AST::ASTException::SIGQUIT: {
                                            throw SQLError("Quit can't be in files");
                                        }
                                        case AST::ASTException::SIGSEL: {
                                            auto select = dynamic_cast<AST::SelectStatement*>(fstatement.get());
                                            cout << select->callT() << endl;
                                            break;
                                        }
                                        case AST::ASTException::SIGEXEC: {
                                            throw SQLError("Exec can't be in files");
                                        }
                                    }
                                }
                            }
                            //------------file end
                            break;
                        }
                    }
                }
            }
        }catch (BaseError& e) {
            cout << e.what() << endl;
        }
    }
    
}

}