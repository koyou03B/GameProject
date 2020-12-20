#pragma once
#include <memory>
#include <vector>
#include <assert.h>
#include <map>
#include ".\LibrarySource\Framework.h"

using ProductCode = uint32_t;
inline ProductCode FindProductCode()
{
	static ProductCode ID = 0;
	int lastID = ID++;
	return lastID;
}
//各商品に対して決まったコードを
//作ってあげる
template <typename T>
inline ProductCode FindProductCode() noexcept
{
	static ProductCode typeID = FindProductCode();
	return typeID;
}

//exsample:
//スーパーにある何々のコーナーにある商品
//イズミヤの鮮魚コーナーにある鮪の鮪にあたる
class ObjectProduct
{
public:
	virtual void Init() = 0;
	virtual void Update(float& elapsedTime) = 0;
	virtual void Render(ID3D11DeviceContext* immediateContext) = 0;

	ObjectProduct() = default;
	virtual ~ObjectProduct() = default;
};

//exsample:
//スーパーにある何々のコーナー全体という認識
//イズミヤの鮮魚コーナーにあたる
class ProductManager
{
public:
	ProductManager() = default;
	~ProductManager() = default;

	//exsample:
	//商品の入荷
	template <typename T, typename...TArgs>
	const T& AddProduct(TArgs&&... mArgs)
	{
		//この形にしていると引数付きにも対応できる
		T* product(new T(std::forward<TArgs>(mArgs)...));
		ProductCode code = FindProductCode<T>();

		m_product.emplace_back(std::make_pair(code, product));
		product->Init();

		return *product;
	};

	//exsample:
	//在庫チェック
	template <typename T>
	bool HasProduct() const
	{
		ProductCode findCode = FindProductCode<T>();
		return FindProductIndex(findCode);
	}

	//exsample:
	//お求めの商品を渡します
	template<typename T>
	T* GiveProduct()
	{
		ProductCode findCode = FindProductCode<T>();
		for (auto& product : m_product)
		{
			if (product.first == findCode)
				return static_cast<T*>(product.second);
		}
		return nullptr;
	}

	void Release()
	{
		m_product.clear();
	}

private:
	//exsample:
	//全商品(鮮魚コーナー)から
	//この商品コードの商品は置いてますか?
	bool FindProductIndex(const ProductCode findCode) const
	{
		for (auto& product : m_product)
		{
			if (product.first == findCode)
				return true;
		}

		return false;
	}
private:
	//exsample:
	//<商品コード,商品>
	using Product = std::pair<ProductCode, ObjectProduct*>;
	std::vector<Product> m_product;
};

//exsample:
//イズミヤ,ライフなどお店に
//何のコーナーを作るかを管理してる
//イズミヤに鮮魚コーナー作りました
class Market
{
public:
	//exsample:
	//このお店にこのコーナーはありますか？
	inline  ProductManager& FindProductConer(uint32_t tag)
	{
		const auto it = m_productConers.find(tag);
		if (it != m_productConers.end())
		{
			return *it->second.get();
		}
		return *static_cast<ProductManager*>(nullptr);
	}

	//exsample:
	//このお店にこのコーナー作りました！
	ProductManager& AddProductConer(uint32_t tag)
	{
		ProductManager* e = new ProductManager();
		std::unique_ptr<ProductManager> uPtr{ e };
		m_productConers.insert(std::make_pair(tag, std::move(uPtr)));
		return *e;
	}

	void Closing()
	{
		if (m_productConers.empty())return;
		for (auto& productConer : m_productConers)
		{
			if(productConer.second)
				productConer.second->Release();
		}
		m_productConers.clear();
	}

private:
	//exsample:
	//<鮮魚,鮮魚コーナーです>
	std::map<uint32_t, std::unique_ptr<ProductManager>> m_productConers;
};

inline Market& RunningMarket()
{
	static Market manager;
	return manager;
}