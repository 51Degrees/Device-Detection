///
/// # BrowserName
///
/// All browsers with a north american market share [greater than %1](https://gs.statcounter.com/browser-market-share/all/north-america)
/// are provided as enumerated values when the feature `browser-name-enum` is enabled.
///
/// ```
/// use fiftyonedegrees_sys::values::browser_name::BrowserName;
///
/// let browser = BrowserName::Chrome;
/// ```
///
/// Any browser with less than 1% market share will appear as "Other", with a full list of possible
/// values available at https://51degrees.com/resources/property-dictionary.
///
/// ```
/// use fiftyonedegrees_sys::values::browser_name::BrowserName;
///
/// let browser = BrowserName::Other("BlackBerry");
/// ```
///
/// Some browsers have multiple variants in the 51degrees dataset such as the "Facebook" browser:
///
/// * Facebook for Android
/// * Facebook for BlackBerry
/// * Facebook for Windows
/// * Facebook for iOS
///
/// Instances where the variant is based on the operating system, the variant will be ignored
/// and the browser will use the "root" enumeration (in this case, Facebook).
///
/// Instances where the variant is based on a specific mode (eg: Edge for Android), the default
/// behavior is to treat it as "Other". If you prefer a type-safe alternative, you can turn on the
/// `browser-enum-mode` feature to enable this functionality:
///
/// ```
/// use fiftyonedegrees_sys::values::browser_name::{BrowserName, BrowserMode};
///
/// let browser = BrowserName::Edge(BrowserMode::Standard);
/// ```
///

#[cfg(not(any(feature = "browser-name-enum", feature = "browser-name-enum-mode")))]
pub type BrowserName<'detector> = &'detector str;

#[cfg(feature = "browser-name-enum-mode")]
mod with_mode;

#[cfg(feature = "browser-name-enum-mode")]
pub use self::with_mode::*;

#[cfg(feature = "browser-name-enum")]
mod without_mode;

#[cfg(feature = "browser-name-enum")]
pub use self::without_mode::*;
