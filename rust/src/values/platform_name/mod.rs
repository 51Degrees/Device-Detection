///
/// # PlatformName
///
/// All operating systems with a north american market share
/// [greater than %1](https://gs.statcounter.com/browser-market-share/all/north-america)
/// are provided as enumerated values.
///
/// ```
/// use fiftyonedegrees_sys::values::platform_name::PlatformName;
///
/// let browser = PlatformName::Windows;
/// ```
///
/// Any browser with less than 1% market share will appear as "Other", with a full list of possible
/// values available at https://51degrees.com/resources/property-dictionary.
///
/// ```
/// use fiftyonedegrees_sys::values::platform_name::PlatformName;
///
/// let browser = PlatformName::Other("BlackBerry OS");
/// ```
///
/// Some operating systems have multiple variants in the 51degrees dataset such as the "Windows":
///
/// * Windows
/// * Windows Mobile
/// * Windows NT
/// * Windows Phone
/// * Windows RT
/// * Windows Server
///
/// In instances where there is a variant, it is returned as an "Other":
///
/// ```
/// use fiftyonedegrees_sys::values::platform_name::PlatformName;
///
/// let browser = PlatformName::Other("Windows Server");
/// ```
///

#[cfg(not(feature = "platform-name-enum"))]
pub type PlatformName<'detector> = &'detector str;

#[cfg(feature = "platform-name-enum")]
mod enumerated;

#[cfg(feature = "platform-name-enum")]
pub use self::enumerated::*;
