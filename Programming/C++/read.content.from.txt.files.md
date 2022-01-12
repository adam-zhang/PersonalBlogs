```
std::ifstream in("some.file");
std::isreambuf_iterator<char> begin(in);
std::isreambuf_iterator<char> end;
std::string some_str(begin, end);
```
```
std::ifstream in("some.file");
std::ostringstream tmp;
tmp << in.rdbuf();
std::string str = tmp.str();
```
