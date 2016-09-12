/*
  This file is part of Freeminer.

  Freeminer is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Freeminer  is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with Freeminer.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <mutex>

#include "exceptions.h"
#include "filesys.h"
#include "key_value_storage.h"
#include "log.h"
#include "util/pointer.h"
#include "util/string.h"

KeyValueStorage::KeyValueStorage(const std::string &savedir, const std::string &name) :
	db(nullptr),
	db_name(name)
{
	fullpath = savedir + DIR_DELIM + db_name + ".db";
	open();
}

bool KeyValueStorage::open() {
#if USE_LEVELDB
	leveldb::Options options;
	options.create_if_missing = true;
	auto status = leveldb::DB::Open(options, fullpath, &db);
	verbosestream<<"KeyValueStorage::open() db_name="<<db_name << " status="<< status.ok()<< " error="<<status.ToString()<<std::endl;
	if (!status.ok()) {
		std::lock_guard<Mutex> lock(mutex);
		error = status.ToString();
		errorstream<< "Trying to repair database ["<<error<<"]"<<std::endl;
		try {
			status = leveldb::RepairDB(fullpath, options);
		} catch (std::exception &e) {
			errorstream<< "First repair exception ["<<e.what()<<"]"<<std::endl;
			auto options_repair = options;
			options_repair.paranoid_checks = true;
			try {
				status = leveldb::RepairDB(fullpath, options_repair);
			} catch (std::exception &e) {
				errorstream<< "Second repair exception ["<<e.what()<<"]"<<std::endl;
			}
		}
		if (!status.ok()) {
			error = status.ToString();
			errorstream<< "Repair fail ["<<error<<"]"<<std::endl;
			db = nullptr;
			return true;
		}
		status = leveldb::DB::Open(options, fullpath, &db);
		if (!status.ok()) {
			error = status.ToString();
			errorstream<< "Trying to reopen database ["<<error<<"]"<<std::endl;
			db = nullptr;
			return true;
		}
	}
#endif
	return false;
}

void KeyValueStorage::close()
{
	if (!db)
		return;
	delete db;
	db = nullptr;
}

KeyValueStorage::~KeyValueStorage()
{
	//errorstream<<"KeyValueStorage::~KeyValueStorage() "<<db_name<<std::endl;
	close();
}

bool KeyValueStorage::put(const std::string &key, const std::string &data)
{
	if (!db)
		return false;
#if USE_LEVELDB
	auto status = db->Put(write_options, key, data);
	if (!status.ok()) {
		std::lock_guard<Mutex> lock(mutex);
		error = status.ToString();
		return false;
	}
#endif
	return true;
}

bool KeyValueStorage::put(const std::string &key, const float &data)
{
	return put(key, ftos(data));
}


bool KeyValueStorage::put_json(const std::string &key, const Json::Value &data)
{
	return put(key, json_writer.write(data).c_str());
}

bool KeyValueStorage::get(const std::string &key, std::string &data)
{
	if (!db)
		return false;
#if USE_LEVELDB
	auto status = db->Get(read_options, key, &data);
	if (!status.ok()) {
		std::lock_guard<Mutex> lock(mutex);
		error = status.ToString();
		return false;
	}
#endif
	return true;
}

bool KeyValueStorage::get(const std::string &key, float &data)
{
	std::string tmpstring;
	if (get(key, tmpstring) && !tmpstring.empty()) {
		data = stof(tmpstring);
		return true;
	}
	return false;
}

bool KeyValueStorage::get_json(const std::string &key, Json::Value & data)
{
	std::string value;
	get(key, value);
	if (value.empty())
		return false;
	return json_reader.parse(value, data);
}

std::string KeyValueStorage::get_error() {
	std::lock_guard<Mutex> lock(mutex);
	return error;
}

bool KeyValueStorage::del(const std::string &key)
{
	if (!db)
		return false;
#if USE_LEVELDB
	//std::lock_guard<Mutex> lock(mutex);
	auto status = db->Delete(write_options, key);
	return status.ok();
#else
	return true;
#endif
}

#if USE_LEVELDB
leveldb::Iterator* KeyValueStorage::new_iterator() {
	if (!db)
		return nullptr;
	return db->NewIterator(read_options);
}
#endif
