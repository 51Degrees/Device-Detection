#[derive(PartialEq, Debug)]
pub enum BrowserName<'detector> {
    Chrome,
    Safari,
    Facebook,
    Samsung,
    InternetExplorer,
    Edge,
    Firefox,
    Other(&'detector str),
}

impl<'detector> From<&'detector str> for BrowserName<'detector> {
    fn from(value: &'detector str) -> Self {
        match value {
            "Chrome" => BrowserName::Chrome,
            "Chrome Mobile" => BrowserName::Chrome,
            "Chrome for iOS" => BrowserName::Chrome,

            "Safari" => BrowserName::Safari,
            "Mobile Safari" => BrowserName::Safari,

            "Facebook for Android" => BrowserName::Facebook,
            "Facebook for BlackBerry" => BrowserName::Facebook,
            "Facebook for Windows" => BrowserName::Facebook,
            "Facebook for iOS" => BrowserName::Facebook,

            "Samsung Browser" => BrowserName::Samsung,

            "Internet Explorer" => BrowserName::InternetExplorer,
            "Internet Explorer Mobile" => BrowserName::InternetExplorer,

            "Edge" => BrowserName::Edge,
            "Edge (Chromium)" => BrowserName::Edge,
            "Edge Mobile" => BrowserName::Edge,
            "Edge for Android" => BrowserName::Edge,
            "Edge for iOS" => BrowserName::Edge,

            "Firefox" => BrowserName::Firefox,
            "Firefox for Mobile" => BrowserName::Firefox,
            "Firefox for iOS" => BrowserName::Firefox,

            _ => BrowserName::Other(value)
        }
    }
}
