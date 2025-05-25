**5/25/2025**
*  **feedback.md is for instructor use only. DO NOT change the feedback.md**; make a copy if needed
* class definition style & standards:  first, member vars  none per line and do not forget to initialize them;  second functions: separate group of functions  by a blank line; either matching setter/getter pairs or list all setters back to back and list all getters back to back; start with a default c’tor, followed by  copy c’tor (if any), then other c’tors in the order of increased number of arguments, d’tor- right after c’tor(s), overloaded functions- list them back to back in the order of increased number of arguments;  all accessor/getter functions should be const; all functions with more than one statement should be normal (defined externally, outside of the class and below main()), no inline functions with more than one statement; initialize all member variables to appropriate default values at the time of definition; all member variables must be private; classes must provide a strong exception guarantee; must have default c’tor; implement Rule of Three when necessary
* inadequate and/ insufficient test data/ not all possible scenarios and edge cases have been tested; e.g., each record has 4 pieces of data; pow(2,4)->16  possible combinations of valid/invalid and only one of them is a valid record; all menu options should be tested;  no zeros, negative numbers, etc; must test all created functions -5
* does not compile -5
* copy c’tors  and = implementation  -5
* 322, 328, 338 and like: must handle exception in  c’tors; should never abruptly terminate c’tor due to an exception; otherwise, you will have a var Id or reference and no object -5
*  360  and like who handles rethrow? -2
* 359 how do you know what happened? 
* belongs to BST class bool buildBSTFromFile(const char* IN_FILE, BST& tree); - 2
* 104 should be a parameter, better yet  const string: e.g. foo(“error”); foor(FAILED_INPUT); -1

* readability, comments; do not state the obvious, do not reiterate/duplicate the code, remove outdated code; comments supposd to help and not create unnecessary clutter; "Code never lies, comments sometimes do." - Ron Jeffries”; what is the purpose of the comments as this(ese) one(s); -1
```text
Node(); //FIXME


