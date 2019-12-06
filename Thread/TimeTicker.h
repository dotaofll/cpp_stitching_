#pragma once

#include <assert.h>
#include "utils.h"

//需要加一个logger

namespace stiching{
	class Ticker {
	public:
		Ticker(int64_t minMs = 0,
			   const char* where = "",
			   bool printLog = false){
			_begin = getNowTime();
			_created = _begin;
			_minMs = minMs;
			_where = where;
		}
		~Ticker() {
			int64_t tm = getNowTime() - _begin;
			if (tm > _minMs) {
				//_stream << _where << " take time:" << tm << endl;
			}
			else {
				//_stream.clear();
			}
		}
		uint64_t elapsedTime() {
			//_stream.clear();
			return getNowTime() - _begin;
		}
		uint64_t createdTime() {
			//_stream.clear();
			return getNowTime() - _created;
		}
		void resetTime() {
			//_stream.clear();
			_begin = getNowTime();
		}

		static uint64_t getNowTime() {
			struct timeval tv;
			gettimeofday(&tv, NULL);
			return tv.tv_sec * 1000 + tv.tv_usec / 1000;
		}
	private:
		uint64_t _begin;
		uint64_t _created;
		const char* _where;
		int64_t _minMs;
	};
}