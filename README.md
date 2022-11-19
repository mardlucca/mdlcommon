# C++ Common Libraries

C++ is a great language, but let's be honest, the Standard Library isn't that great. Essentially,
IMHO, it still lacks a lot of things that other languages provide natively. And while there are
third party libraries available out there for a lot of different things, I don't know, I like to
write my own, it allows me for a great deal of learning, among other things.

This is my take at a bunch of common code that can facilitate writing C++ systems. This is, as much
as anything else, a learning exercise and today there are already a bunch of things that I **don't** 
like in this code base. Among other things:
* I need to standardize on using std::size_t for index values. 
* Use std types for integral values (e.g std::int32_t, etc). Right now I simply use native types like
int, long, float, double, etc, assuming sizes provided in a 64bit system.
* Better support for i18n. Today I basically use char and std::string for most stuff. I times I will 
use templated types and wchar/wstring, but not normally.
* Better use of constness and const expressions. I'm now usingn c++20, and constexpr are used 
everywhere now in the standard library. Might as well take advantage.
