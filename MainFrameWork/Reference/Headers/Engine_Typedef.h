#ifndef Engine_Typedef_h__
#define Engine_Typedef_h__

namespace Engine
{
	typedef		bool						_bool;

	typedef		signed char					_byte;
	typedef		unsigned char				_ubyte;

	typedef		char						_char;
	typedef		unsigned char				_uchar;
	typedef		wchar_t						_tchar;
	
	typedef		signed short				_short;
	typedef		unsigned short				_ushort;

	

	typedef		signed int					_int;
	typedef		unsigned int				_uint;

	typedef		signed long					_long;
	typedef		unsigned long				_ulong;

	typedef		float						_float;
	typedef		double						_double;	



	using int8 = __int8;
	using int16 = __int16;
	using int32 = __int32;
	using int64 = __int64;
	using uint8 = unsigned __int8;
	using uint16 = unsigned __int16;
	using uint32 = unsigned __int32;
	using uint64 = unsigned __int64;

	using Vec2 = DirectX::SimpleMath::Vector2;
	using Vec3 = DirectX::SimpleMath::Vector3;
	using Vec4 = DirectX::SimpleMath::Vector4;
	using Matrix = DirectX::SimpleMath::Matrix;
	using Quaternion = DirectX::SimpleMath::Quaternion;

}

#endif // Engine_Typedef_h__
