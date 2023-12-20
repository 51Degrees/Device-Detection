#[derive(PartialEq, Debug)]
pub enum BrowserMode {
    Standard,
    Chrome,
    Headless,
    MobileAsDesktop,
    VirtualReality
}

#[derive(PartialEq, Debug)]
pub enum BrowserName<'detector> {
    Chrome(BrowserMode),
    Safari,
    Facebook,
    Samsung(BrowserMode),
    InternetExplorer(BrowserMode),
    Edge(BrowserMode),
    Firefox(BrowserMode),
    Other(&'detector str),
}

impl<'detector> From<&str> for BrowserName<'detector> {
    fn from(value: &str) -> Self {
        match value {
            "Chrome" => BrowserName::Chrome(BrowserMode::Standard),
            "Chrome (Mobile in desktop mode)" => BrowserName::Chrome(BrowserMode::MobileAsDesktop),
            "Chrome Headless" => BrowserName::Chrome(BrowserMode::Headless),
            "Chrome Mobile Headless" => BrowserName::Chrome(BrowserMode::Headless),
            "Chrome Mobile" => BrowserName::Chrome(BrowserMode::Standard),
            "Chrome for iOS" => BrowserName::Chrome(BrowserMode::Standard),

            "Safari" => BrowserName::Safari,
            "Mobile Safari" => BrowserName::Safari,

            "Facebook for Android" => BrowserName::Facebook,
            "Facebook for BlackBerry" => BrowserName::Facebook,
            "Facebook for Windows" => BrowserName::Facebook,
            "Facebook for iOS" => BrowserName::Facebook,

            "Samsung Browser" => BrowserName::Samsung(BrowserMode::Standard),
            "Samsung Browser (Mobile in desktop mode)" => BrowserName::Samsung(BrowserMode::MobileAsDesktop),
            "Samsung Browser (in VR mode)" => BrowserName::Samsung(BrowserMode::VirtualReality),

            "Internet Explorer" => BrowserName::InternetExplorer(BrowserMode::Standard),
            "Internet Explorer Mobile" => BrowserName::InternetExplorer(BrowserMode::Standard),
            "Internet Explorer (Mobile in desktop mode)" => BrowserName::InternetExplorer(BrowserMode::MobileAsDesktop),
            "Internet Explorer with Chrome Frame" => BrowserName::InternetExplorer(BrowserMode::Chrome),

            "Edge" => BrowserName::Edge(BrowserMode::Standard),
            "Edge (Mobile in desktop mode)" => BrowserName::Edge(BrowserMode::MobileAsDesktop),
            "Edge (Chromium)" => BrowserName::Edge(BrowserMode::Chrome),
            "Edge Mobile" => BrowserName::Edge(BrowserMode::Standard),
            "Edge for Android" => BrowserName::Edge(BrowserMode::Standard),
            "Edge for Android (Mobile in desktop mode)" => BrowserName::Edge(BrowserMode::MobileAsDesktop),
            "Edge for iOS" => BrowserName::Edge(BrowserMode::Standard),
            "Edge for iOS (Mobile in desktop mode)" => BrowserName::Edge(BrowserMode::MobileAsDesktop),

            "Firefox" => BrowserName::Firefox(BrowserMode::Standard),
            "Firefox for Mobile" => BrowserName::Firefox(BrowserMode::Standard),
            "Firefox for Mobile (in VR mode)" => BrowserName::Firefox(BrowserMode::VirtualReality),
            "Firefox for iOS" => BrowserName::Firefox(BrowserMode::Standard),

            _ => BrowserName::Other(value)
        }
    }
}
