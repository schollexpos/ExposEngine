#pragma once
#include "stdafx.h" 

namespace expos {
	namespace compiler {

		/** A type an Variable can have.
		*
		*	EVT_NULL only gets set in an error.
		*/
		enum VarType {
			EVT_INT,
			EVT_FLOAT,
			EVT_CHAR,
			EVT_BOOL,
			EVT_STRING,
			EVT_ARRAY,
			EVT_NULL
		};

		union VarValue {
			int v_int;
			float v_float;
			bool v_bool;
			char v_char;
			char *v_string;
		};

		struct Variable {
			std::string name;
			VarType type;
			VarValue var;

			Variable(int v) {
				type = EVT_INT;
				var.v_int = v;
			}
			Variable(char v) {
				type = EVT_CHAR;
				var.v_char = v;
			}
			Variable(float v) {
				type = EVT_FLOAT;
				var.v_float = v;
			}
			Variable(bool v) {
				type = EVT_BOOL;
				var.v_bool = v;
			}
			Variable(const std::string& v) {
				var.v_string = nullptr;
				this->setString(v);
			}

			void setString(const std::string& str) {
				if (this->var.v_string != nullptr) {
					delete[] var.v_string;
				}
				var.v_string = new char[str.length() + 1];
				for (int i = 0; i < str.length(); i++) {
					var.v_string[i] = str[i];
				}
				type = EVT_STRING;
			}

			std::string getString() const {
				return std::string(var.v_string);
			}

			std::string getStringDel() {
				std::string str = this->getString();
				this->deleteString();
				return str;
			}

			void deleteString() {
				delete[] var.v_string;
				var.v_string = nullptr;
			}

			bool inited() {
				return type != EVT_NULL;
			}
		};


		namespace Parser {
			struct Structure {
				std::string name;
				std::vector<Variable> members;
			};

			struct Function {
				std::string name;
				std::vector<Variable> arguments;
				Variable returnValue;

			};


			class Parser {

			};

		}

		



	}

}