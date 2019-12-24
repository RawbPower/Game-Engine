#pragma once

namespace ge {

	// Pure virtual interface
	class VertexBuffer 
	{
	public:
		virtual ~VertexBuffer() {}

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		// Use this instead of constructor
		static VertexBuffer* Create(float* vertices, uint32_t size);
	};

	class IndexBuffer 
	{
	public:
		virtual ~IndexBuffer() {}

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		virtual uint32_t GetCount() const = 0;

		// Use this instead of constructor
		static IndexBuffer* Create(uint32_t* indices, uint32_t count);
	};

}
