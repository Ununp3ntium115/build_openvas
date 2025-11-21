//! CVSS Score Calculation
//!
//! Implements CVSS v3.x base score calculation according to the official specification.

use crate::types::*;

/// Calculate CVSS v3.x Base Score
///
/// Based on CVSS v3.1 specification:
/// https://www.first.org/cvss/v3.1/specification-document
pub fn calculate_cvss_v3_base_score(metrics: &CvssV3Base) -> f64 {
    // Impact Sub-Score calculation
    let isc_base = 1.0 - (
        (1.0 - confidentiality_impact(&metrics.confidentiality)) *
        (1.0 - integrity_impact(&metrics.integrity)) *
        (1.0 - availability_impact(&metrics.availability))
    );

    let impact = if metrics.scope == "U" {
        6.42 * isc_base
    } else {
        7.52 * (isc_base - 0.029) - 3.25 * (isc_base - 0.02).powi(15)
    };

    // Exploitability Sub-Score calculation
    let exploitability = 8.22 *
        attack_vector_value(&metrics.attack_vector) *
        attack_complexity_value(&metrics.attack_complexity) *
        privileges_required_value(&metrics.privileges_required, &metrics.scope) *
        user_interaction_value(&metrics.user_interaction);

    // Base Score calculation
    if impact <= 0.0 {
        0.0
    } else {
        let score = if metrics.scope == "U" {
            (impact + exploitability).min(10.0)
        } else {
            (1.08 * (impact + exploitability)).min(10.0)
        };
        roundup(score)
    }
}

/// Parse CVSS v3.x vector string
///
/// Example: "CVSS:3.1/AV:N/AC:L/PR:N/UI:N/S:C/C:H/I:H/A:H"
pub fn parse_cvss_v3_vector(vector: &str) -> Result<CvssV3Base, String> {
    let parts: Vec<&str> = vector.split('/').collect();

    if parts.is_empty() || !parts[0].starts_with("CVSS:3") {
        return Err("Invalid CVSS v3 vector string".to_string());
    }

    let mut metrics = CvssV3Base {
        attack_vector: String::new(),
        attack_complexity: String::new(),
        privileges_required: String::new(),
        user_interaction: String::new(),
        scope: String::new(),
        confidentiality: String::new(),
        integrity: String::new(),
        availability: String::new(),
    };

    for part in &parts[1..] {
        let kv: Vec<&str> = part.split(':').collect();
        if kv.len() != 2 {
            continue;
        }

        match kv[0] {
            "AV" => metrics.attack_vector = kv[1].to_string(),
            "AC" => metrics.attack_complexity = kv[1].to_string(),
            "PR" => metrics.privileges_required = kv[1].to_string(),
            "UI" => metrics.user_interaction = kv[1].to_string(),
            "S" => metrics.scope = kv[1].to_string(),
            "C" => metrics.confidentiality = kv[1].to_string(),
            "I" => metrics.integrity = kv[1].to_string(),
            "A" => metrics.availability = kv[1].to_string(),
            _ => {}
        }
    }

    // Validate all required metrics are present
    if metrics.attack_vector.is_empty() ||
       metrics.attack_complexity.is_empty() ||
       metrics.privileges_required.is_empty() ||
       metrics.user_interaction.is_empty() ||
       metrics.scope.is_empty() ||
       metrics.confidentiality.is_empty() ||
       metrics.integrity.is_empty() ||
       metrics.availability.is_empty() {
        return Err("Missing required CVSS metrics".to_string());
    }

    Ok(metrics)
}

/// Create full CvssV3 from vector string
pub fn cvss_v3_from_vector(vector: &str) -> Result<CvssV3, String> {
    let metrics = parse_cvss_v3_vector(vector)?;
    let base_score = calculate_cvss_v3_base_score(&metrics);
    let severity = CvssSeverity::from_score(base_score);

    Ok(CvssV3 {
        base_metrics: metrics,
        base_score,
        temporal_score: None,
        environmental_score: None,
        severity,
        vector_string: vector.to_string(),
    })
}

// Metric value functions
fn attack_vector_value(av: &str) -> f64 {
    match av {
        "N" => 0.85,  // Network
        "A" => 0.62,  // Adjacent
        "L" => 0.55,  // Local
        "P" => 0.2,   // Physical
        _ => 0.0,
    }
}

fn attack_complexity_value(ac: &str) -> f64 {
    match ac {
        "L" => 0.77,  // Low
        "H" => 0.44,  // High
        _ => 0.0,
    }
}

fn privileges_required_value(pr: &str, scope: &str) -> f64 {
    match (pr, scope) {
        ("N", _) => 0.85,     // None
        ("L", "U") => 0.62,   // Low, Unchanged
        ("L", "C") => 0.68,   // Low, Changed
        ("H", "U") => 0.27,   // High, Unchanged
        ("H", "C") => 0.50,   // High, Changed
        _ => 0.0,
    }
}

fn user_interaction_value(ui: &str) -> f64 {
    match ui {
        "N" => 0.85,  // None
        "R" => 0.62,  // Required
        _ => 0.0,
    }
}

fn confidentiality_impact(c: &str) -> f64 {
    match c {
        "H" => 0.56,  // High
        "L" => 0.22,  // Low
        "N" => 0.0,   // None
        _ => 0.0,
    }
}

fn integrity_impact(i: &str) -> f64 {
    match i {
        "H" => 0.56,  // High
        "L" => 0.22,  // Low
        "N" => 0.0,   // None
        _ => 0.0,
    }
}

fn availability_impact(a: &str) -> f64 {
    match a {
        "H" => 0.56,  // High
        "L" => 0.22,  // Low
        "N" => 0.0,   // None
        _ => 0.0,
    }
}

/// Round up to one decimal place per CVSS spec
fn roundup(value: f64) -> f64 {
    (value * 10.0).ceil() / 10.0
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_parse_cvss_v3_vector() {
        let vector = "CVSS:3.1/AV:N/AC:L/PR:N/UI:N/S:C/C:H/I:H/A:H";
        let metrics = parse_cvss_v3_vector(vector).unwrap();

        assert_eq!(metrics.attack_vector, "N");
        assert_eq!(metrics.attack_complexity, "L");
        assert_eq!(metrics.privileges_required, "N");
        assert_eq!(metrics.user_interaction, "N");
        assert_eq!(metrics.scope, "C");
        assert_eq!(metrics.confidentiality, "H");
        assert_eq!(metrics.integrity, "H");
        assert_eq!(metrics.availability, "H");
    }

    #[test]
    fn test_calculate_critical_score() {
        // CVE-2021-44228 (Log4Shell) - Critical
        let vector = "CVSS:3.1/AV:N/AC:L/PR:N/UI:N/S:C/C:H/I:H/A:H";
        let cvss = cvss_v3_from_vector(vector).unwrap();

        assert_eq!(cvss.base_score, 10.0);
        assert_eq!(cvss.severity, CvssSeverity::Critical);
    }

    #[test]
    fn test_calculate_high_score() {
        // Typical high severity vulnerability
        let vector = "CVSS:3.1/AV:N/AC:L/PR:N/UI:N/S:U/C:H/I:H/A:H";
        let cvss = cvss_v3_from_vector(vector).unwrap();

        assert_eq!(cvss.base_score, 9.8);
        assert_eq!(cvss.severity, CvssSeverity::Critical);
    }

    #[test]
    fn test_calculate_medium_score() {
        // Medium severity vulnerability
        let vector = "CVSS:3.1/AV:N/AC:L/PR:L/UI:R/S:U/C:L/I:L/A:L";
        let cvss = cvss_v3_from_vector(vector).unwrap();

        assert!(cvss.base_score >= 4.0 && cvss.base_score < 7.0);
        assert_eq!(cvss.severity, CvssSeverity::Medium);
    }

    #[test]
    fn test_invalid_vector() {
        let result = parse_cvss_v3_vector("invalid");
        assert!(result.is_err());
    }

    #[test]
    fn test_roundup() {
        assert_eq!(roundup(7.33), 7.4);
        assert_eq!(roundup(4.0), 4.0);
        assert_eq!(roundup(5.567), 5.6);
    }
}
