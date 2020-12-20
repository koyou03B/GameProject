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
//�e���i�ɑ΂��Č��܂����R�[�h��
//����Ă�����
template <typename T>
inline ProductCode FindProductCode() noexcept
{
	static ProductCode typeID = FindProductCode();
	return typeID;
}

//exsample:
//�X�[�p�[�ɂ��鉽�X�̃R�[�i�[�ɂ��鏤�i
//�C�Y�~���̑N���R�[�i�[�ɂ��閎�̖��ɂ�����
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
//�X�[�p�[�ɂ��鉽�X�̃R�[�i�[�S�̂Ƃ����F��
//�C�Y�~���̑N���R�[�i�[�ɂ�����
class ProductManager
{
public:
	ProductManager() = default;
	~ProductManager() = default;

	//exsample:
	//���i�̓���
	template <typename T, typename...TArgs>
	const T& AddProduct(TArgs&&... mArgs)
	{
		//���̌`�ɂ��Ă���ƈ����t���ɂ��Ή��ł���
		T* product(new T(std::forward<TArgs>(mArgs)...));
		ProductCode code = FindProductCode<T>();

		m_product.emplace_back(std::make_pair(code, product));
		product->Init();

		return *product;
	};

	//exsample:
	//�݌Ƀ`�F�b�N
	template <typename T>
	bool HasProduct() const
	{
		ProductCode findCode = FindProductCode<T>();
		return FindProductIndex(findCode);
	}

	//exsample:
	//�����߂̏��i��n���܂�
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
	//�S���i(�N���R�[�i�[)����
	//���̏��i�R�[�h�̏��i�͒u���Ă܂���?
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
	//<���i�R�[�h,���i>
	using Product = std::pair<ProductCode, ObjectProduct*>;
	std::vector<Product> m_product;
};

//exsample:
//�C�Y�~��,���C�t�Ȃǂ��X��
//���̃R�[�i�[����邩���Ǘ����Ă�
//�C�Y�~���ɑN���R�[�i�[���܂���
class Market
{
public:
	//exsample:
	//���̂��X�ɂ��̃R�[�i�[�͂���܂����H
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
	//���̂��X�ɂ��̃R�[�i�[���܂����I
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
	//<�N��,�N���R�[�i�[�ł�>
	std::map<uint32_t, std::unique_ptr<ProductManager>> m_productConers;
};

inline Market& RunningMarket()
{
	static Market manager;
	return manager;
}