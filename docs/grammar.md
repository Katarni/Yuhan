
\<program> ::= \<includes> \<namespaces> \<programBody>  
\<includes> ::= {`#include` \<literalString>}  
\<namespaces> ::= {`using` \<namespaceDefinition>}
\<namespaceDefinition> ::= \<identifier> {`.` \<identifier>}  
\<programBody> ::= {(\<function> | \<varDefinition>) | \<struct>}  
\<struct> ::= `struct` \<identifier> `{` \<programBody> `};`  
\<type> ::= \[`const`] \<identifier>  
\<varDefinition> ::= \<type> (\<identifier> | \<identifier> `=` \<exp12>) {`,`  (\<identifier> | \<identifier> `=` \<exp12>)}`;`  
\<funcVarDefinition> ::= \<type> \<identifier> {`,` \<type> \<identifier>} {`,` \<type> \<identifier> `=` \<exp12>} | \[\<type> \<identifier> `=` \<exp12> {`,` \<type> \<identifier> `=` \<exp12>}]  