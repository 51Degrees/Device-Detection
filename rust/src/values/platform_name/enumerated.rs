#[derive(PartialEq, Debug)]
pub enum PlatformName<'detector> {
    Android,
    MacOS,
    iOS,
    Windows,
    Other(&'detector str)
}

impl<'detector> From<&'detector str> for PlatformName<'detector> {
    fn from(value: &'detector str) -> Self {
        match value {
            "Android" => PlatformName::Android,
            "macOS" => PlatformName::MacOS,
            "iOS" => PlatformName::iOS,
            "Windows" => PlatformName::Windows,
            _ => PlatformName::Other(value)
        }
    }
}