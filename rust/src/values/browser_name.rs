#[derive(PartialEq, Debug)]
pub enum BrowserName{
    Chrome,
    SafariMobile,
    Facebook,
    Safari,
    Webkit,
    Samsung,
    Firefox,
    Edge,
    InternetExplorer,
    ChromeWebview,
    Silk,
    Opera,
}

impl BrowserName {
    pub fn from_str(value: &str) -> Option<BrowserName>{
        match value{
            "Chrome" => Some(BrowserName::Chrome),
            "SafariMobile" => Some(BrowserName::SafariMobile),
            "Facebook" => Some(BrowserName::Facebook),
            "Safari" => Some(BrowserName::Safari),
            "Webkit" => Some(BrowserName::Webkit),
            "Samsung" => Some(BrowserName::Samsung),
            "Firefox" => Some(BrowserName::Firefox),
            "Edge" => Some(BrowserName::Edge),
            "InternetExplorer" => Some(BrowserName::InternetExplorer),
            "ChromeWebview" => Some(BrowserName::ChromeWebview),
            "Silk" => Some(BrowserName::Silk),
            "Opera" => Some(BrowserName::Opera),
            _ => None
        }
    }
}