// Powered by nChain's Nakasendo libraries.
// See LICENSE.txt in project root for licensing information.

/*
 * Helper for controlling the format objects are serialised in.
 */

#ifndef _NCHAIN_SDK_SERIALISATION_FORMAT_H_
#define _NCHAIN_SDK_SERIALISATION_FORMAT_H_

#include <impl/utils/EnumCast.h>

namespace nakasendo
{

/// A simple class for describing the required serialisation format.
struct SerialisationFormat
{
    /// Enumerate the possible serialisation styles.
    enum class Style
    {
        DEFAULT,
        WIF
    };

    /// Enumerate the serialisation versions.
    enum class Version
    {
        SDK1
    };

    /**
    * Constructor required for non-standards compliant Windows compiler.
    * @param style The serialisation style.
    * @param version The serialisation format version.
    */
    SerialisationFormat(Style style = Style::DEFAULT, Version version = Version::SDK1)
        : mStyle{style}, mVersion{version} {}

    /// The style to use.
    Style mStyle { Style::DEFAULT };

    /// The version.
    Version mVersion { Version::SDK1 };
};

/// Enable enum_cast for Style
inline const impl::utils::enumTableT<SerialisationFormat::Style>&
    enumTable(SerialisationFormat::Style)
{
    static impl::utils::enumTableT<SerialisationFormat::Style> table
    {   
        {SerialisationFormat::Style::DEFAULT, "Default"},
        {SerialisationFormat::Style::WIF,     "Wif"}
    };
    return table;
}

/// Enable enum_cast for Version
inline const impl::utils::enumTableT<SerialisationFormat::Version>&
    enumTable(SerialisationFormat::Version)
{
    static impl::utils::enumTableT<SerialisationFormat::Version> table
    {   
        {SerialisationFormat::Version::SDK1, "SDK 1"}
    };
    return table;
}

}

#endif
