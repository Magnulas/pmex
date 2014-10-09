/**
 * Wrapper class
 * 
 * At points we need to wrap primitive data types in classes, so that we can
 * pass them as template arguments to classes that like to inherit from their
 * template arguments--this is done in CohomologyPersistence, for example.
 */

template<typename Primitive>
class    Wrapper
{

    public:

        Wrapper () {}
        Wrapper (Primitive v)                       { value = v;    }

        void       setValue  (const Primitive &v)   { value = v;    }
        Primitive &getValue  ()                     { return value; }

        /* provide seemless integration */
        Wrapper   &operator =(const Primitive &v)   { setValue(v);     return *this; }
        operator  Primitive()                       { return getValue;               }

    protected:

        Primitive value;

};

/**
 * IndirectIndexComparison class
 * 
 * This class serves as a comparison function for arrays that are being sorted
 * even though they only contain *indices* to the real data. Therefore, a reference
 * to the original data as well as the data comparison function needs to be passed
 * to this class for it to be functional.
 */

template<class DataContainer, class DataComparison>
class IndirectIndexComparison
{

    public:

        IndirectIndexComparison(const DataContainer &dstor, const DataComparison &dcmp) :
            container(dstor), comparison(dcmp) { }

        bool operator()(const unsigned &idx_1, const unsigned &idx_2) const
        {
            return comparison(container[idx_1], container[idx_2]);
        }

    private:

        const DataContainer  &container;
        const DataComparison &comparison;

};
