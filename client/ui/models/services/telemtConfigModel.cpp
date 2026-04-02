#include "telemtConfigModel.h"

#include <QRegularExpression>

#include "core/qrCodeUtils.h"
#include "protocols/protocols_defs.h"
#include "qrcodegen.hpp"

using namespace amnezia;

TelemtConfigModel::TelemtConfigModel(QObject *parent) : QAbstractListModel(parent)
{
}

int TelemtConfigModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 1;
}

bool TelemtConfigModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid() || index.row() != 0) {
        return false;
    }

    switch (role) {
    case Roles::PortRole: {
        m_protocolConfig.insert(config_key::port, value.toString());
        break;
    }
    case Roles::SecretRole: {
        m_protocolConfig.insert(protocols::telemt::secretKey, value.toString());
        break;
    }
    case Roles::TagRole: {
        m_protocolConfig.insert(protocols::telemt::tagKey, value.toString());
        break;
    }
    case Roles::IsEnabledRole: {
        m_protocolConfig.insert(protocols::telemt::isEnabledKey, value.toBool());
        break;
    }
    case Roles::PublicHostRole: {
        m_protocolConfig.insert(protocols::telemt::publicHostKey, value.toString());
        break;
    }
    case Roles::TransportModeRole: {
        m_protocolConfig.insert(protocols::telemt::transportModeKey, value.toString());
        break;
    }
    case Roles::TlsDomainRole: {
        m_protocolConfig.insert(protocols::telemt::tlsDomainKey, value.toString());
        break;
    }
    case Roles::AdditionalSecretsRole: {
        m_protocolConfig.insert(protocols::telemt::additionalSecretsKey,
                                QJsonArray::fromStringList(value.toStringList()));
        break;
    }
    case Roles::WorkersModeRole: {
        m_protocolConfig.insert(protocols::telemt::workersModeKey, value.toString());
        break;
    }
    case Roles::WorkersRole: {
        m_protocolConfig.insert(protocols::telemt::workersKey, value.toString());
        break;
    }
    case Roles::NatEnabledRole: {
        m_protocolConfig.insert(protocols::telemt::natEnabledKey, value.toBool());
        break;
    }
    case Roles::NatInternalIpRole: {
        m_protocolConfig.insert(protocols::telemt::natInternalIpKey, value.toString());
        break;
    }
    case Roles::NatExternalIpRole: {
        m_protocolConfig.insert(protocols::telemt::natExternalIpKey, value.toString());
        break;
    }
    case Roles::MaskEnabledRole: {
        m_protocolConfig.insert(protocols::telemt::maskEnabledKey, value.toBool());
        break;
    }
    case Roles::UseMiddleProxyRole: {
        m_protocolConfig.insert(protocols::telemt::useMiddleProxyKey, value.toBool());
        break;
    }
    case Roles::TlsEmulationRole: {
        m_protocolConfig.insert(protocols::telemt::tlsEmulationKey, value.toBool());
        break;
    }
    case Roles::UserNameRole: {
        m_protocolConfig.insert(protocols::telemt::userNameKey, value.toString());
        break;
    }
    default: {
        return false;
    }
    }

    emit dataChanged(index, index, QList { role });
    return true;
}

QVariant TelemtConfigModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() != 0) {
        return QVariant();
    }

    switch (role) {
    case Roles::PortRole: {
        return m_protocolConfig.value(config_key::port).toString(protocols::telemt::defaultPort);
    }
    case Roles::SecretRole: {
        return m_protocolConfig.value(protocols::telemt::secretKey).toString();
    }
    case Roles::TagRole: {
        return m_protocolConfig.value(protocols::telemt::tagKey).toString();
    }
    case Roles::TgLinkRole: {
        return m_protocolConfig.value(protocols::telemt::tgLinkKey).toString();
    }
    case Roles::TmeLinkRole: {
        return m_protocolConfig.value(protocols::telemt::tmeLinkKey).toString();
    }
    case Roles::IsEnabledRole: {
        return m_protocolConfig.value(protocols::telemt::isEnabledKey).toBool(true);
    }
    case Roles::PublicHostRole: {
        return m_protocolConfig.value(protocols::telemt::publicHostKey)
                .toString(m_fullConfig.value(config_key::hostName).toString());
    }
    case Roles::TransportModeRole: {
        return m_protocolConfig.value(protocols::telemt::transportModeKey)
                .toString(protocols::telemt::transportModeStandard);
    }
    case Roles::TlsDomainRole: {
        return m_protocolConfig.value(protocols::telemt::tlsDomainKey).toString();
    }
    case Roles::AdditionalSecretsRole: {
        QJsonArray arr = m_protocolConfig.value(protocols::telemt::additionalSecretsKey).toArray();
        QStringList list;
        for (const auto &v : arr) {
            list << v.toString();
        }
        return list;
    }
    case Roles::WorkersModeRole: {
        return m_protocolConfig.value(protocols::telemt::workersModeKey).toString(protocols::telemt::workersModeAuto);
    }
    case Roles::WorkersRole: {
        return m_protocolConfig.value(protocols::telemt::workersKey).toString(protocols::telemt::defaultWorkers);
    }
    case Roles::NatEnabledRole: {
        return m_protocolConfig.value(protocols::telemt::natEnabledKey).toBool(false);
    }
    case Roles::NatInternalIpRole: {
        return m_protocolConfig.value(protocols::telemt::natInternalIpKey).toString();
    }
    case Roles::NatExternalIpRole: {
        return m_protocolConfig.value(protocols::telemt::natExternalIpKey).toString();
    }
    case Roles::MaskEnabledRole: {
        return m_protocolConfig.value(protocols::telemt::maskEnabledKey).toBool(false);
    }
    case Roles::UseMiddleProxyRole: {
        return m_protocolConfig.value(protocols::telemt::useMiddleProxyKey).toBool(false);
    }
    case Roles::TlsEmulationRole: {
        return m_protocolConfig.value(protocols::telemt::tlsEmulationKey).toBool(false);
    }
    case Roles::UserNameRole: {
        return m_protocolConfig.value(protocols::telemt::userNameKey)
                .toString(QString::fromUtf8(protocols::telemt::defaultUserName));
    }
    }

    return QVariant();
}

void TelemtConfigModel::updateModel(const QJsonObject &config)
{
    beginResetModel();

    m_fullConfig = config;
    QJsonObject protocolConfig = config.value(config_key::telemt).toObject();

    m_protocolConfig.insert(config_key::port,
                            protocolConfig.value(config_key::port).toString(protocols::telemt::defaultPort));
    m_protocolConfig.insert(protocols::telemt::secretKey,
                            protocolConfig.value(protocols::telemt::secretKey).toString());
    m_protocolConfig.insert(protocols::telemt::tagKey, protocolConfig.value(protocols::telemt::tagKey).toString());
    m_protocolConfig.insert(protocols::telemt::tgLinkKey,
                            protocolConfig.value(protocols::telemt::tgLinkKey).toString());
    m_protocolConfig.insert(protocols::telemt::tmeLinkKey,
                            protocolConfig.value(protocols::telemt::tmeLinkKey).toString());
    m_protocolConfig.insert(protocols::telemt::isEnabledKey,
                            protocolConfig.value(protocols::telemt::isEnabledKey).toBool(true));
    m_protocolConfig.insert(protocols::telemt::publicHostKey,
                            protocolConfig.value(protocols::telemt::publicHostKey).toString());
    m_protocolConfig.insert(
            protocols::telemt::transportModeKey,
            protocolConfig.value(protocols::telemt::transportModeKey).toString(protocols::telemt::transportModeStandard));
    m_protocolConfig.insert(protocols::telemt::tlsDomainKey,
                            protocolConfig.value(protocols::telemt::tlsDomainKey).toString());
    m_protocolConfig.insert(protocols::telemt::maskEnabledKey,
                            protocolConfig.value(protocols::telemt::maskEnabledKey).toBool(false));
    m_protocolConfig.insert(protocols::telemt::tlsEmulationKey,
                            protocolConfig.value(protocols::telemt::tlsEmulationKey).toBool(false));
    m_protocolConfig.insert(protocols::telemt::useMiddleProxyKey,
                            protocolConfig.value(protocols::telemt::useMiddleProxyKey).toBool(false));
    m_protocolConfig.insert(protocols::telemt::userNameKey,
                            protocolConfig.value(protocols::telemt::userNameKey)
                                    .toString(QString::fromUtf8(protocols::telemt::defaultUserName)));
    m_protocolConfig.insert(protocols::telemt::additionalSecretsKey,
                            protocolConfig.value(protocols::telemt::additionalSecretsKey).toArray());
    m_protocolConfig.insert(
            protocols::telemt::workersModeKey,
            protocolConfig.value(protocols::telemt::workersModeKey).toString(protocols::telemt::workersModeAuto));
    m_protocolConfig.insert(
            protocols::telemt::workersKey,
            protocolConfig.value(protocols::telemt::workersKey).toString(protocols::telemt::defaultWorkers));
    m_protocolConfig.insert(protocols::telemt::natEnabledKey,
                            protocolConfig.value(protocols::telemt::natEnabledKey).toBool(false));
    m_protocolConfig.insert(protocols::telemt::natInternalIpKey,
                            protocolConfig.value(protocols::telemt::natInternalIpKey).toString());
    m_protocolConfig.insert(protocols::telemt::natExternalIpKey,
                            protocolConfig.value(protocols::telemt::natExternalIpKey).toString());

    endResetModel();
}

QJsonObject TelemtConfigModel::getConfig()
{
    m_fullConfig.insert(config_key::telemt, m_protocolConfig);
    return m_fullConfig;
}

void TelemtConfigModel::generateSecret()
{
    // 16 random bytes = 32 hex chars (same contract as MTProxy / Telemt configure_container.sh)
    QString secret;
    for (int i = 0; i < 16; ++i) {
        quint32 byte = QRandomGenerator::global()->bounded(256);
        secret += QString("%1").arg(byte, 2, 16, QChar('0'));
    }

    m_protocolConfig.insert(protocols::telemt::secretKey, secret);
    emit dataChanged(index(0), index(0), QList<int> { SecretRole });
}

void TelemtConfigModel::setSecret(const QString &secret)
{
    if (secret.isEmpty()) {
        return;
    }
    setData(index(0), secret, SecretRole);
}

bool TelemtConfigModel::validateAndSetSecret(const QString &rawSecret)
{
    // Must be exactly 32 hex chars (matches MTProxy and server-side grep in telemt/configure_container.sh)
    if (!QRegularExpression(QStringLiteral("^[0-9a-fA-F]{32}$")).match(rawSecret).hasMatch()) {
        return false;
    }
    setData(index(0), rawSecret, SecretRole);
    return true;
}

void TelemtConfigModel::setPort(const QString &port)
{
    setData(index(0), port, PortRole);
}

void TelemtConfigModel::setTag(const QString &tag)
{
    setData(index(0), tag, TagRole);
}

void TelemtConfigModel::setPublicHost(const QString &host)
{
    setData(index(0), host, PublicHostRole);
}

void TelemtConfigModel::setTransportMode(const QString &mode)
{
    setData(index(0), mode, TransportModeRole);
}

QString TelemtConfigModel::getTransportMode() const
{
    return m_protocolConfig.value(protocols::telemt::transportModeKey).toString(protocols::telemt::transportModeStandard);
}

QString TelemtConfigModel::getTlsDomain() const
{
    return m_protocolConfig.value(protocols::telemt::tlsDomainKey)
            .toString(QString::fromUtf8(protocols::telemt::defaultTlsDomain));
}

QString TelemtConfigModel::getPublicHost() const
{
    return m_protocolConfig.value(protocols::telemt::publicHostKey).toString();
}

void TelemtConfigModel::setTlsDomain(const QString &domain)
{
    setData(index(0), domain, TlsDomainRole);
}

void TelemtConfigModel::setWorkersMode(const QString &mode)
{
    setData(index(0), mode, WorkersModeRole);
}

void TelemtConfigModel::setWorkers(const QString &workers)
{
    setData(index(0), workers, WorkersRole);
}

void TelemtConfigModel::setNatEnabled(bool enabled)
{
    setData(index(0), enabled, NatEnabledRole);
}

void TelemtConfigModel::setNatInternalIp(const QString &ip)
{
    setData(index(0), ip, NatInternalIpRole);
}

void TelemtConfigModel::setNatExternalIp(const QString &ip)
{
    setData(index(0), ip, NatExternalIpRole);
}

void TelemtConfigModel::setMaskEnabled(bool enabled)
{
    setData(index(0), enabled, MaskEnabledRole);
}

void TelemtConfigModel::setUseMiddleProxy(bool enabled)
{
    setData(index(0), enabled, UseMiddleProxyRole);
}

void TelemtConfigModel::setTlsEmulation(bool enabled)
{
    setData(index(0), enabled, TlsEmulationRole);
}

void TelemtConfigModel::setUserName(const QString &name)
{
    setData(index(0), name, UserNameRole);
}

void TelemtConfigModel::addAdditionalSecret()
{
    QString newSecret;
    for (int i = 0; i < 16; ++i) {
        quint32 byte = QRandomGenerator::global()->bounded(256);
        newSecret += QString("%1").arg(byte, 2, 16, QChar('0'));
    }

    QJsonArray arr = m_protocolConfig.value(protocols::telemt::additionalSecretsKey).toArray();
    arr.append(newSecret);
    m_protocolConfig.insert(protocols::telemt::additionalSecretsKey, arr);
    emit dataChanged(index(0), index(0), QList<int> { AdditionalSecretsRole });
}

void TelemtConfigModel::removeAdditionalSecret(int idx)
{
    QJsonArray arr = m_protocolConfig.value(protocols::telemt::additionalSecretsKey).toArray();
    if (idx < 0 || idx >= arr.size()) {
        return;
    }
    arr.removeAt(idx);
    m_protocolConfig.insert(protocols::telemt::additionalSecretsKey, arr);
    emit dataChanged(index(0), index(0), QList<int> { AdditionalSecretsRole });
}

void TelemtConfigModel::setEnabled(bool enabled)
{
    m_protocolConfig.insert(protocols::telemt::isEnabledKey, enabled);
    emit dataChanged(index(0), index(0), QList<int> { IsEnabledRole });
}

QString TelemtConfigModel::generateQrCode(const QString &text)
{
    if (text.isEmpty()) {
        return "";
    }
    auto qr = qrCodeUtils::generateQrCode(text.toUtf8());
    return qrCodeUtils::svgToBase64(QString::fromStdString(toSvgString(qr, 1)));
}

QString TelemtConfigModel::defaultTlsDomain() const
{
    return QString::fromUtf8(protocols::telemt::defaultTlsDomain);
}

QString TelemtConfigModel::defaultPort() const
{
    return QString::fromUtf8(protocols::telemt::defaultPort);
}

QString TelemtConfigModel::defaultWorkers() const
{
    return QString::fromUtf8(protocols::telemt::defaultWorkers);
}

int TelemtConfigModel::maxWorkers() const
{
    return protocols::telemt::maxWorkers;
}

QString TelemtConfigModel::transportModeStandard() const
{
    return QString::fromUtf8(protocols::telemt::transportModeStandard);
}

QString TelemtConfigModel::transportModeFakeTLS() const
{
    return QString::fromUtf8(protocols::telemt::transportModeFakeTLS);
}

QString TelemtConfigModel::workersModeAuto() const
{
    return QString::fromUtf8(protocols::telemt::workersModeAuto);
}

QString TelemtConfigModel::workersModeManual() const
{
    return QString::fromUtf8(protocols::telemt::workersModeManual);
}

QHash<int, QByteArray> TelemtConfigModel::roleNames() const
{
    QHash<int, QByteArray> roles;

    roles[PortRole] = "port";
    roles[SecretRole] = "secret";
    roles[TagRole] = "tag";
    roles[TgLinkRole] = "tgLink";
    roles[TmeLinkRole] = "tmeLink";
    roles[IsEnabledRole] = "isEnabled";
    roles[PublicHostRole] = "publicHost";
    roles[TransportModeRole] = "transportMode";
    roles[TlsDomainRole] = "tlsDomain";
    roles[AdditionalSecretsRole] = "additionalSecrets";
    roles[WorkersModeRole] = "workersMode";
    roles[WorkersRole] = "workers";
    roles[NatEnabledRole] = "natEnabled";
    roles[NatInternalIpRole] = "natInternalIp";
    roles[NatExternalIpRole] = "natExternalIp";
    roles[MaskEnabledRole] = "maskEnabled";
    roles[UseMiddleProxyRole] = "useMiddleProxy";
    roles[TlsEmulationRole] = "tlsEmulation";
    roles[UserNameRole] = "userName";

    return roles;
}
