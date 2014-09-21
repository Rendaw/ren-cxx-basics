#ifndef chain_h
#define chain_h

template <typename DataT> struct ChainT
{
	private:
		struct LinkT
		{
			std::shared_ptr<LinkT<DataT>> const Parent;
			DataT const Data;
		};

		size_t Depth;
		std::shared_ptr<LinkT<DataT>> Head;

	public:
		ChainT(void) : Depth(0) {}

		ChainT(ChainT<DataT> &Other, DataT &&Data) : 
			Depth(Other.Depth + 1), 
			Head(std::make_shared<LinkT>(Other.Head, std::move(Data)) 
			{}

		ChainT(ChainT<DataT> &Other, DataT const &Data) : 
			Depth(Other.Depth + 1), 
			Head(std::make_shared<LinkT>(Other.Head, Data) 
			{}

		ChainT(ChainT<DataT> const &Other) : Depth(Other.Depth), Head(Other.Head) {}

		ChainT(ChainT<DataT> &&Other) : Depth(Other.Depth), Head(std::move(Other.Head)) {}

		ChainT<DataT> &operator =(ChainT<DataT> const &Other) 
			{ Depth = Other.Depth; Head = Other.Head; return *this; }

		ChainT<DataT> &operator =(ChainT<DataT> &&Other)
			{ Depth = Other.Depth; Head = std::move(Other.Head); return *this; }

		size_t GetDepth(void) const { return Depth; }

		template <typename FunctionT> void Compile(FunctionT const &Function)
		{
			std::list<LinkT *> Links;
			if (!Head) return;
			auto *Next = Head.get();
			while (Next)
			{
				Links.push_front(Next);
				Next = Next->Parent.get();
			}
			for (auto &Link : Links) Function(Link->Data);
		}
};

#endif
