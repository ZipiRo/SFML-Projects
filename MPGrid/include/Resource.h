template <typename T>
class Resource
{
private:
    std::unordered_map<std::string, T> data;

public:
    Resource() {}

    void Insert(std::string name, T &value)
    {   
        data[name] = value;
    }

    bool Exists(std::string name)
    {
        return data.find(name) != data.end();
    }

    T &Get(std::string name)
    {
        return data.at(name);
    }
};