#pragma once

#ifdef timeout
#undef timeout
#endif

#include <cstdint>
#include <iostream>
#include <vector>
#include <bsoncxx/json.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/stdx.hpp>
#include <mongocxx/uri.hpp>
#include <mongocxx/instance.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/string/to_string.hpp>


//#include <CMAKE_CONSTANTS.h>

#include <vector>
#include <optional>
#include <utility>
#include <memory>


namespace MongoDB {

class DataBaseEnviroment {
public:
    DataBaseEnviroment(const std::string& host_path) {
        mongocxx::instance inst{};
        mongocxx::uri uri(host_path);
        client_ = mongocxx::client(uri);
    }
    
    mongocxx::database GetDataBase(const std::string& data_base_name) {
        return client_[data_base_name];
    }
    
private:
    mongocxx::client client_;
};

class DataBase : public DataBaseEnviroment {
public:
    
    DataBase(const std::string& host_path, const std::string& data_base_name) : DataBaseEnviroment(host_path) {
        data_base = GetDataBase(data_base_name);
    }
    
    mongocxx::collection GetCollection(const std::string& collection_name) {
        return data_base[collection_name];
    }
private:
    mongocxx::database data_base;
};

class Collection : public DataBase {
public:
    
    Collection(const std::string& host_path, const std::string& data_base_name, const std::string& collection_name): DataBase(host_path, data_base_name) {
        collection_ = GetCollection(collection_name);
    }
    
    void Add(const std::string& json_string) {
        bsoncxx::document::value input_doc_value = bsoncxx::from_json(json_string);
        collection_.insert_one(input_doc_value.view());
    }
    
    std::optional<bsoncxx::document::value> FindDocByKeyValue(const std::string& key, const std::string& value) {
        using bsoncxx::builder::stream::document;
        using bsoncxx::builder::stream::finalize;
        bsoncxx::stdx::optional<bsoncxx::document::value> maybe_result = collection_.find_one( document{} << key << value << finalize);
        if(maybe_result) {
            return maybe_result.get();
        } else return std::nullopt;
    }
    
    std::optional<std::string> FindJsonByKeyValue(const std::string& key, const std::string& value) {
        using bsoncxx::builder::stream::document;
        using bsoncxx::builder::stream::finalize;
        bsoncxx::stdx::optional<bsoncxx::document::value> maybe_result = collection_.find_one( document{} << key << value << finalize);
        if(maybe_result) {
            return bsoncxx::to_json(maybe_result.get().view());
        } else return std::nullopt;
    }
    
    std::vector<std::string> FindAllValuesByKey(const std::string& key) {
        std::vector<std::string> all_values;
        mongocxx::cursor cursor = collection_.find({});
        for(auto doc : cursor) {
            all_values.push_back(GetValueByKeyInDoc(key, doc));
        }
        return all_values;
    }
    
    std::string GetValueByKeyInDoc(const std::string& key, const bsoncxx::document::value& document) {
        bsoncxx::document::element element = document.view()[key];
        if (element) {
            return bsoncxx::string::to_string(element.get_utf8().value);
        } else return "";
    }
    std::string GetValueByKeyInDoc(const std::string& key, const bsoncxx::document::view& document_view) {
        bsoncxx::document::element element = document_view[key];
        if (element) {
            return bsoncxx::string::to_string(element.get_utf8().value);
        } else return "";
    }
    
    
    
private:
    mongocxx::collection collection_;
};


}

