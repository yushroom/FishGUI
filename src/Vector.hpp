#pragma once
#include <cmath>
#include <ostream>

namespace FishGUI
{
	template<typename T>
	struct Vector2
	{
		T x, y;
		
		friend std::ostream& operator<<(std::ostream& os, const Vector2& v)
		{
			os << "Vector2(" << v.x << ", " << v.y << ")\n";
			return os;
		}
	};
	
	typedef Vector2<int> Vector2i;
	typedef Vector2<float> Vector2f;
	
	template<typename T>
	struct Vector3
	{
		T x, y, z;
		
		friend std::ostream& operator<<(std::ostream& os, const Vector3& v)
		{
			os << "Vector3(" << v.x << ", " << v.y << ", " << v.z << ")\n";
			return os;
		}
	};
	
	typedef Vector3<int> Vector3i;
	typedef Vector3<float> Vector3f;
	
	struct Size
	{
		int width, height;
		
		friend std::ostream& operator<<(std::ostream& os, const Size& s)
		{
			os << "Size(" << s.width << ", " << s.height << ")\n";
			return os;
		}
	};
	
	struct Rect
	{
		int x, y, width, height;
		
		friend std::ostream& operator<<(std::ostream& os, const Rect& r)
		{
			os << "Rect(" << r.x << ", " << r.y << ", " << r.width << ", " << r.height << ")\n";
			return os;
		}
	};

	inline bool PointInRect(int px, int py, int x, int y, int w, int h)
	{
		return (px > x && px < x + w && py > y && py < y + h);
	}

	inline bool PointInRect(int px, int py, Rect const & rect)
	{
		return PointInRect(px, py, rect.x, rect.y, rect.width, rect.height);
	}
}
