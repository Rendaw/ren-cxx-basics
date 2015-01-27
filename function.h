#ifndef ren_function
#define ren_function
#include <stdexcept>
#include <utility>
#include <cassert>
#include <cstring>
#include <type_traits>

// Notes
// std::function can't hold lambdas that capture non-copyable types.  This is a proper reimplementation (although labmdas only).

template <typename signature_type>
struct function;

template <typename return_type, typename ...arg_types>
struct function<return_type(arg_types ...)>
{
	protected:
		void *data;
		void (*delete_data)(void *data);
		void *(*copy_data)(void *data);
		return_type (*call_data)(void *data, arg_types ...args);

		template <typename lambda_type>
		void set_copy_function(typename std::enable_if<std::is_copy_constructible<lambda_type>::value>::type* = nullptr)
		{
			copy_data = [](void *data) -> void *
			{
				return new lambda_type(*reinterpret_cast<lambda_type *>(data));
			};
		}

		template <typename lambda_type>
		void set_copy_function(typename std::enable_if<!std::is_copy_constructible<lambda_type>::value>::type* = nullptr)
		{
			copy_data = nullptr;
		}

		template <typename lambda_type> 
		void set_functions(void)
		{
			delete_data = [](void *data) 
			{ 
				delete reinterpret_cast<lambda_type *>(data); 
			};

			call_data = [](void *data, arg_types ...args)
			{ 
				return reinterpret_cast<lambda_type *>(data)->
					operator()(std::forward<arg_types>(args)...); 
			};

			set_copy_function<lambda_type>();
		}
		
		template <typename lambda_type> 
		void set_no_clear(lambda_type const &lambda)
		{
			data = new lambda_type(lambda);
			set_functions<lambda_type>();
		}

		template <typename lambda_type> 
		void set_no_clear(lambda_type &&lambda)
		{
			data = new lambda_type(std::move(lambda));
			set_functions<lambda_type>();
		}
		
		void set_no_clear(function<return_type(arg_types...)> const &other) 
		{
			if (!other.copy_data) throw std::runtime_error("Function has no copy constructor.");
			data = other.copy_data(other.data);
			delete_data = other.delete_data;
			copy_data = other.copy_data;
			call_data = other.call_data;
		}
		
		void set_no_clear(function<return_type(arg_types...)> &&other) 
		{
			data = other.data;
			delete_data = other.delete_data;
			copy_data = other.copy_data;
			call_data = other.call_data;
			memset(&other, 0, sizeof(other));
		}

	public:
		// CONSTRUCTORS
		function(void) : data(nullptr), delete_data(nullptr), copy_data(nullptr), call_data(nullptr) {}

		template <typename lambda_type> 
		function(lambda_type const &lambda) 
			{ set_no_clear(lambda); }

		template <typename lambda_type> 
		function(lambda_type &&lambda) 
			{ set_no_clear(std::move(lambda)); }
		
		function(function<return_type(arg_types...)> const &other) 
			{ set_no_clear(other); }

		function(function<return_type(arg_types...)> &&other) 
			{ set_no_clear(std::move(other)); }

		// DESTRUCTOR
		~function(void) { clear(); }

		// OPERATORS
		template <typename lambda_type> 
		function<return_type(arg_types...)> &operator =(lambda_type const &lambda)
			{ set(lambda); return *this; }
		
		template <typename lambda_type> 
		function<return_type(arg_types...)> &operator =(lambda_type &&lambda)
			{ set(std::move(lambda)); return *this; }
		
		function<return_type(arg_types...)> &operator =(function<return_type(arg_types...)> const &other) 
			{ set(other); return *this; }
		
		function<return_type(arg_types...)> &operator =(function<return_type(arg_types...)> const &&other) 
			{ set(std::move(other)); return *this; }

		operator bool(void) const { return data; }
		
		return_type operator ()(arg_types ...args)
		{
			if (!data) throw std::runtime_error("Calling unset function.");
			return call_data(data, std::forward<arg_types>(args)...);
		}
		
		return_type operator ()(arg_types ...args) const
		{
			if (!data) throw std::runtime_error("Calling unset function.");
			return call_data(data, std::forward<arg_types>(args)...);
		}

		// METHODS
		template <typename lambda_type> 
		void set(lambda_type const &lambda)
			{ clear(); set_no_clear(lambda); }

		template <typename lambda_type> 
		void set(lambda_type &&lambda)
			{ clear(); set_no_clear(std::move(lambda)); }
		
		void set(function<return_type(arg_types...)> const &other) 
			{ clear(); set_no_clear(other); }
		
		void set(function<return_type(arg_types...)> &&other) 
			{ clear(); set_no_clear(std::move(other)); }

		void clear(void)
		{
			if (!data) return;
			delete_data(data);
			memset(this, 0, sizeof(*this));
		}
};

#endif

